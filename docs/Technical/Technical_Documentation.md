# 基于OpenGL的大规模自然场景实时漫游系统 - 技术文档

## 1. 系统概述

### 1.1 项目背景

本系统基于 C++ 与 Modern OpenGL (Core Profile 4.5+) 构建，实现了一个具备"游戏级"视觉表现与实时交互能力的室外场景漫游系统。

### 1.2 技术栈

| 类别 | 技术/工具 | 版本 |
|------|----------|------|
| 编程语言 | C++ | 17 |
| 图形API | OpenGL | Core Profile 4.5+ |
| 着色语言 | GLSL | 4.50 |
| 窗口管理 | GLFW | 3.x |
| 数学库 | GLM | - |
| 图像加载 | stb_image | - |
| UI框架 | Dear ImGui | - |
| 开发环境 | Visual Studio | 2022 |

---

## 2. 系统架构

### 2.1 分层架构图

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│              (main.cpp - 程序入口与Game Loop)                │
├─────────────────────────────────────────────────────────────┤
│                     Editor Layer                            │
│          (ImGui 编辑器 - 参数调节、性能监控)                   │
├───────────────┬─────────────────┬───────────────────────────┤
│   Terrain     │   Water         │   Environment             │
│   System      │   System        │   System                  │
│  (地形渲染)    │  (水体仿真)      │  (天空盒/光照)             │
├───────────────┴─────────────────┴───────────────────────────┤
│                    Core Engine Layer                        │
│    Camera │ Shader │ Texture │ Mesh │ Cubemap               │
├─────────────────────────────────────────────────────────────┤
│                    Platform Layer                           │
│              GLFW │ GLAD │ GLM │ stb_image                  │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 模块职责

| 模块 | 职责 |
|------|------|
| Application | 窗口创建、OpenGL上下文、主循环、输入处理 |
| RoamingApp | 具体应用逻辑、场景管理、渲染流程 |
| Camera | 第一人称摄像机、视图矩阵计算 |
| Shader | GLSL着色器编译、链接、Uniform管理 |
| Texture | 2D纹理加载、参数设置、Mipmap |
| Mesh | VAO/VBO/EBO封装、顶点布局 |
| Cubemap | 立方体贴图加载 |
| Terrain | 高度图解析、网格生成、渲染 |
| Frustum | 视锥体剔除、6平面提取 |
| TerrainChunk | 地形块、多LOD网格 |
| ChunkedTerrain | 分块管理、剔除+LOD选择 |
| Water | 水面网格、反射折射渲染 |
| WaterFramebuffers | FBO管理、反射折射纹理 |
| Skybox | 天空盒渲染 |
| Lighting | 日夜循环、光照参数计算 |
| SceneSettings | 场景参数保存/加载 |

---

## 3. 核心模块实现

### 3.1 地形系统 (Terrain System)

#### 3.1.1 高度图加载

**原理**：读取灰度PNG图像，每个像素的灰度值 (0-255) 映射为高度值。

**关键代码流程**：
1. 使用 `stb_image` 加载图像
2. 遍历像素，将灰度值归一化到 [0, 1]
3. 存储为二维高度数组

**支持特性**：
- 任意尺寸高度图 (推荐 512×512)
- 双线性插值获取任意坐标高度

#### 3.1.2 网格生成

**算法**：
1. 根据高度图尺寸创建顶点网格
2. 每个顶点包含：位置、法线、纹理坐标
3. 法线通过相邻顶点高度差计算
4. 使用索引缓冲 (EBO) 生成三角形

**顶点数据结构**：
```cpp
struct Vertex {
    glm::vec3 position;  // 世界坐标位置
    glm::vec3 normal;    // 法线向量
    glm::vec2 texCoord;  // 纹理坐标
};
```

#### 3.1.3 多纹理混合

**混合策略**：基于高度 + 坡度

| 纹理 | 条件 |
|------|------|
| 草地 | 低海拔 + 平坦区域 |
| 岩石 | 中海拔 + 陡峭坡度 |
| 雪地 | 高海拔区域 |

**着色器实现要点**：
```glsl
// 坡度计算
float slope = 1.0 - dot(normal, vec3(0.0, 1.0, 0.0));

// 权重计算 (使用 smoothstep 平滑过渡)
float grassWeight = smoothstep(...);
float rockWeight = ...;
float snowWeight = ...;

// 混合纹理
vec3 albedo = grassColor * grassWeight + 
              rockColor * rockWeight + 
              snowColor * snowWeight;
```

---

### 3.2 水体系统 (Water System)

#### 3.2.1 反射/折射原理

**实现步骤**：
1. **反射Pass**：将摄像机位置关于水面对称，渲染到反射FBO
2. **折射Pass**：正常摄像机，裁剪水面以上部分，渲染到折射FBO
3. **水面Pass**：使用反射/折射纹理渲染水面

#### 3.2.2 裁剪平面技术

使用 `gl_ClipDistance` 实现硬件级裁剪：

```glsl
// 顶点着色器
uniform vec4 uClipPlane;
gl_ClipDistance[0] = dot(worldPos, uClipPlane);

// 反射时: uClipPlane = vec4(0, 1, 0, -waterHeight)  // 裁剪水下
// 折射时: uClipPlane = vec4(0, -1, 0, waterHeight)  // 裁剪水上
```

#### 3.2.3 Fresnel 效果

**原理**：视线与水面夹角越小，反射越强

```glsl
float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
refractiveFactor = pow(refractiveFactor, 0.5);
FragColor = mix(reflectColor, refractColor, refractiveFactor);
```

#### 3.2.4 DuDv 波浪扭曲

使用 DuDv 贴图对纹理坐标进行偏移，产生动态波纹效果：

```glsl
vec2 distortion = (texture(uDudvMap, texCoord).rg * 2.0 - 1.0) * waveStrength;
reflectTexCoord += distortion;
refractTexCoord += distortion;
```

---

### 3.3 天空盒系统 (Skybox System)

#### 3.3.1 实现原理

1. 使用立方体贴图 (Cubemap) 存储6个方向的天空图像
2. 渲染一个包围摄像机的立方体
3. 移除视图矩阵的平移分量，确保天空盒始终"无限远"

#### 3.3.2 深度测试技巧

```glsl
// 顶点着色器：将深度设为1.0 (最远)
gl_Position = pos.xyww;  // z = w, 透视除法后 z/w = 1.0
```

```cpp
// 渲染时使用 GL_LEQUAL
glDepthFunc(GL_LEQUAL);
```

#### 3.3.3 动态颜色混合

天空盒颜色随日夜循环变化：

```glsl
uniform vec3 uSkyColor;
uniform float uBlendFactor;
vec3 finalColor = mix(cubemapColor, uSkyColor, uBlendFactor);
```

---

### 3.4 光照系统 (Lighting System)

#### 3.4.1 日夜循环时间模型

| 时间 | 太阳角度 | 效果 |
|------|----------|------|
| 6:00 | 0° (地平线) | 日出 |
| 12:00 | 90° (正上方) | 正午 |
| 18:00 | 180° (地平线) | 日落 |
| 0:00 | -90° (地下) | 午夜 |

#### 3.4.2 太阳位置计算

```cpp
float angle = (timeOfDay - 6.0f) / 12.0f * PI;
glm::vec3 sunDir = glm::normalize(glm::vec3(
    cos(angle),   // x: 东西方向
    sin(angle),   // y: 高度
    -0.3f         // z: 略微偏移
));
```

#### 3.4.3 动态光照颜色

| 时段 | 太阳颜色 | 环境光颜色 | 天空颜色 |
|------|----------|------------|----------|
| 正午 | 白色 (1.0, 1.0, 0.95) | 蓝灰 | 天蓝 |
| 日落 | 橙色 (1.0, 0.6, 0.3) | 暖褐 | 橙红 |
| 夜晚 | 蓝色 (0.2, 0.2, 0.4) | 深蓝 | 深蓝黑 |

---

### 3.5 LOD 系统 (Level of Detail)

#### 3.5.1 分块架构

地形被分割为 N×N 个块，每块独立管理：

```
ChunkedTerrain
    ├── Frustum (视锥体剔除)
    └── TerrainChunk[] (N×N 个地形块)
            └── LOD Meshes[4] (每块 4 级精度)
```

#### 3.5.2 LOD 级别

| LOD | 顶点步长 | 相对三角形数 | 适用距离 |
|-----|---------|-------------|----------|
| 0 | 1 | 100% | 0-100m |
| 1 | 2 | 25% | 100-200m |
| 2 | 4 | 6.25% | 200-400m |
| 3 | 8 | 1.56% | 400m+ |

#### 3.5.3 LOD 选择算法

```cpp
int calculateLOD(float distance) {
    if (distance < lodDistances[0]) return 0;
    if (distance < lodDistances[1]) return 1;
    if (distance < lodDistances[2]) return 2;
    return 3;
}
```

---

### 3.6 视锥体剔除 (Frustum Culling)

#### 3.6.1 平面提取

从 ViewProjection 矩阵提取 6 个裁剪平面：

```cpp
void Frustum::update(const glm::mat4& vp) {
    // Left plane
    m_planes[LEFT] = glm::vec4(
        vp[0][3] + vp[0][0],
        vp[1][3] + vp[1][0],
        vp[2][3] + vp[2][0],
        vp[3][3] + vp[3][0]
    );
    // ... 其他5个平面类似
}
```

#### 3.6.2 AABB 可见性测试

```cpp
bool Frustum::isBoxVisible(const glm::vec3& min, const glm::vec3& max) {
    for (int i = 0; i < 6; i++) {
        // 找到离平面最近的点 (p-vertex)
        glm::vec3 pVertex;
        pVertex.x = (plane.x >= 0) ? max.x : min.x;
        pVertex.y = (plane.y >= 0) ? max.y : min.y;
        pVertex.z = (plane.z >= 0) ? max.z : min.z;
        
        // 如果最近点在平面外，整个AABB不可见
        if (dot(plane.xyz, pVertex) + plane.w < 0)
            return false;
    }
    return true;
}
```

#### 3.6.3 性能提升

| 场景 | 无优化 | 有剔除+LOD | 提升 |
|------|--------|------------|------|
| 512×512 全可见 | 520K三角形 | ~150K | 3.5× |
| 512×512 部分可见 | 520K三角形 | ~80K | 6.5× |

---

## 4. 渲染管线

### 4.1 每帧渲染流程

```
1. 更新逻辑 (deltaTime)
   ├── 处理输入
   ├── 更新摄像机
   └── 更新光照时间

2. 水体预渲染 (如果启用水体)
   ├── 绑定反射FBO
   │   ├── 计算反射摄像机位置
   │   ├── 设置裁剪平面 (裁剪水下)
   │   └── 渲染场景 (天空盒、地形)
   │
   └── 绑定折射FBO
       ├── 设置裁剪平面 (裁剪水上)
       └── 渲染场景 (地形)

3. 主场景渲染
   ├── 渲染天空盒
   ├── 渲染地形
   └── 渲染水面 (使用反射/折射纹理)

4. UI渲染
   └── ImGui面板
```

---

## 5. 代码统计

### 5.1 源代码行数

#### C++ 源文件 (src/)

| 模块 | 文件 | 行数 |
|------|------|------|
| **Application** | Application.cpp | 223 |
| | Application.h | 70 |
| | RoamingApp.cpp | 565 |
| | RoamingApp.h | 80 |
| | main.cpp | 6 |
| **小计** | | **944** |
| **Core** | Camera.h | 94 |
| | Shader.cpp/h | 164 |
| | Texture.cpp/h | 151 |
| | Mesh.cpp/h | 295 |
| | Cubemap.cpp/h | 157 |
| | stb_image_impl.cpp | 2 |
| **小计** | | **863** |
| **Terrain** | HeightmapLoader.cpp/h | 98 |
| | Terrain.cpp/h | 177 |
| **小计** | | **275** |
| **Water** | Water.cpp/h | 187 |
| | WaterFramebuffers.cpp/h | 184 |
| **小计** | | **371** |
| **Environment** | Skybox.cpp/h | 148 |
| | Lighting.cpp/h | 198 |
| **小计** | | **346** |
| **Editor** | SceneSettings.cpp/h | 177 |
| **小计** | | **177** |
| **Terrain优化** | Frustum.cpp/h | 84 |
| | TerrainChunk.cpp/h | 247 |
| | ChunkedTerrain.cpp/h | 203 |
| **小计** | | **534** |
| **C++ 总计** | | **3,510** |

#### GLSL 着色器 (shaders/)

| 着色器 | 行数 |
|--------|------|
| terrain.vert/frag | 114 |
| water.vert/frag | 127 |
| skybox.vert/frag | 31 |
| test.vert/frag | 34 |
| **着色器总计** | **306** |

#### 总代码量

| 类别 | 行数 |
|------|------|
| C++ 源代码 | 3,510 |
| GLSL 着色器 | 306 |
| **项目总计** | **3,816** |

### 5.2 第三方库使用

| 库 | 用途 | 集成方式 |
|----|------|----------|
| GLFW | 窗口/输入管理 | 静态链接 |
| GLAD | OpenGL函数加载 | 源码集成 |
| GLM | 数学运算 | 头文件库 |
| stb_image | 图像加载 | 单头文件 |
| Dear ImGui | 编辑器UI | 源码集成 |

---

## 6. 性能指标

### 6.1 测试环境

| 项目 | 配置 |
|------|------|
| CPU | (填写你的CPU型号) |
| GPU | (填写你的GPU型号) |
| 内存 | (填写内存大小) |
| 分辨率 | 1280×720 |

### 6.2 性能数据

| 测试场景 | 帧率 (FPS) | 顶点数 | 三角形数 |
|----------|------------|--------|----------|
| 512×512 地形 + 水体 + 天空盒 | (填写) | 262,144 | ~520,000 |
| 仅地形 | (填写) | 262,144 | ~520,000 |
| 日夜循环运行中 | (填写) | - | - |

---

## 7. 操作说明

### 7.1 控制方式

| 按键 | 功能 |
|------|------|
| W/A/S/D | 前后左右移动 |
| Shift | 加速移动 |
| 鼠标移动 | 视角旋转 |
| 滚轮 | 调整视野 (FOV) |
| Space | 切换鼠标捕获 |
| F1 | 切换线框模式 |
| ESC | 退出程序 |

### 7.2 编辑器面板

- **Performance**：FPS、顶点数、GPU信息
- **Camera**：位置、速度、地面行走模式
- **Terrain**：纹理平铺、混合参数
- **Water**：水面高度、波浪参数、颜色
- **Lighting**：时间控制、日夜循环

---

## 8. 编译与部署

### 8.1 编译要求

- Visual Studio 2022
- Windows SDK 10.0+
- OpenGL 4.5+ 支持的显卡

### 8.2 编译步骤

1. 打开 `RoamingSystem.sln`
2. 选择 `Release | x64` 配置
3. 生成解决方案

### 8.3 打包部署

运行 `scripts/package.bat` 自动打包，输出到 `RoamingSystem_Release/` 目录。

---

---

## 9. 美术资源指南

### 9.1 高度图 (Heightmap)

| 来源 | 说明 |
|------|------|
| **Terrain.party** | ⭐ 真实地形数据，可下载任意地球位置 |
| **Textures.com** | 免费高度图，16位灰度 |
| **World Machine** | 专业地形生成软件 |

### 9.2 地形纹理

| 来源 | 说明 |
|------|------|
| **Poly Haven** | ⭐ 完全免费 PBR 纹理 https://polyhaven.com |
| **ambientCG** | CC0 协议，可商用 https://ambientcg.com |

**需要纹理**：草地、岩石、雪地 (Diffuse + 可选 Normal Map)

### 9.3 天空盒

| 来源 | 说明 |
|------|------|
| **Humus** | 经典免费天空盒 http://www.humus.name |
| **OpenGameArt** | 游戏级天空盒 |

### 9.4 水体纹理

- **DuDv Map**：扭曲效果
- **Normal Map**：波光粼粼效果

搜索 "water dudv map" 或 "water normal map" 获取免费素材。

---

*文档版本: v1.1*
*最后更新: 2026-01-13*
