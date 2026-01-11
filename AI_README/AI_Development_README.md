# 基于OpenGL的大规模自然场景实时漫游系统 - 开发文档

## 📋 项目概述

### 课题名称
基于OpenGL的大规模自然场景实时漫游系统的设计与实现

### 选题背景
随着虚拟现实与开放世界游戏的兴起，对大规模自然场景的渲染需求日益增长。尽管 Unity、UE 等商业引擎提供了成熟方案，但其高度封装的黑盒特性往往掩盖了底层渲染管线的运行机制，导致开发者在面对特定的大规模场景优化时，难以在显存管理与 Draw Call 优化上进行深度定制。

### 选题意义
本课题旨在基于 C++ 与 OpenGL 图形标准，从零构建一个具备"游戏级"视觉表现与实时交互能力的室外场景漫游系统。

### 技术栈
| 类别 | 技术 |
|------|------|
| 开发环境 | Visual Studio (C++ 17) |
| 图形接口 | Modern OpenGL (Core Profile 4.5+) |
| 着色语言 | GLSL |
| 窗口管理 | GLFW |
| 数学运算 | GLM |
| 纹理加载 | stb_image |
| UI界面 | ImGui |

---

## 🏗️ 系统架构设计

### 分层架构图

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
│    Camera │ Shader │ Texture │ Mesh │ ResourceManager       │
├─────────────────────────────────────────────────────────────┤
│                    Platform Layer                           │
│              GLFW │ GLAD │ GLM │ stb_image                  │
└─────────────────────────────────────────────────────────────┘
```

### 当前目录结构 (已创建)

```
RoamingSystem/
├── src/                            ✅ 已创建
│   ├── main.cpp                    ✅ 已简化 (7行)
│   ├── Application.h/cpp           ✅ 已完成
│   ├── RoamingApp.h/cpp            ✅ 已完成
│   │
│   ├── Core/                       ✅ 已创建
│   │   ├── Camera.h                ✅ 已迁移
│   │   ├── Shader.h/cpp            ✅ 已迁移
│   │   ├── stb_image_impl.cpp      ✅ 已迁移
│   │   ├── Texture.h/cpp           ✅ 已完成
│   │   ├── TextureArray.h/cpp      ⏳ 待开发
│   │   ├── Mesh.h/cpp              ✅ 已完成
│   │   ├── Transform.h             ⏳ 待开发
│   │   └── ResourceManager.h/cpp   ⏳ 待开发
│   │
│   ├── Terrain/                    ✅ 已创建 (空)
│   │   ├── Terrain.h/cpp           ⏳ 待开发
│   │   ├── HeightmapLoader.h/cpp   ⏳ 待开发
│   │   ├── TerrainChunk.h/cpp      ⏳ 待开发
│   │   ├── LODManager.h/cpp        ⏳ 加分项
│   │   └── FrustumCuller.h/cpp     ⏳ 加分项
│   │
│   ├── Water/                      ✅ 已创建 (空)
│   │   ├── Water.h/cpp             ⏳ 待开发
│   │   ├── WaterFramebuffers.h/cpp ⏳ 待开发
│   │   └── WaveGenerator.h/cpp     ⏳ 待开发
│   │
│   ├── Environment/                ✅ 已创建 (空)
│   │   ├── Skybox.h/cpp            ⏳ 待开发
│   │   ├── Sun.h/cpp               ⏳ 待开发
│   │   └── Lighting.h/cpp          ⏳ 待开发
│   │
│   └── Editor/                     ✅ 已创建 (空)
│       ├── EditorUI.h/cpp          ⏳ 待开发
│       ├── TerrainPanel.h/cpp      ⏳ 待开发
│       ├── WaterPanel.h/cpp        ⏳ 待开发
│       └── LightingPanel.h/cpp     ⏳ 待开发
│
├── shaders/                        ✅ 已有
│   ├── test.vert/frag              ✅ 已有 (示例)
│   ├── terrain.vert/frag           ⏳ 待开发
│   ├── water.vert/frag             ⏳ 待开发
│   ├── skybox.vert/frag            ⏳ 待开发
│   └── common/                     ✅ 已创建 (空)
│       └── lighting.glsl           ⏳ 待开发
│
├── assets/                         ✅ 已创建
│   ├── heightmaps/                 ✅ 已创建 (空)
│   ├── textures/                   ✅ 已创建
│   │   ├── terrain/                ✅ 已创建 (空)
│   │   └── water/                  ✅ 已创建 (空)
│   └── skybox/                     ✅ 已创建 (空)
│
├── imgui/                          ✅ 第三方库
└── AI_README/                      ✅ 开发文档
```

---

## 📊 当前项目进度

### 已完成模块

| 模块 | 状态 | 说明 |
|------|------|------|
| **GLFW 窗口系统** | ✅ 已完成 | OpenGL 4.5 Core Profile |
| **GLAD 加载器** | ✅ 已完成 | 函数指针加载 |
| **Shader 类** | ✅ 已完成 | 基础着色器加载、编译、Uniform设置 |
| **Camera 类** | ✅ 已完成 | 第一人称摄像机、键鼠控制 |
| **ImGui 集成** | ✅ 已完成 | 基础 UI、FPS 显示、颜色编辑器 |
| **Game Loop** | ✅ 已完成 | deltaTime 计算、帧率控制 |
| **示例渲染** | ✅ 已完成 | 带颜色的立方体 |
| **项目结构重构** | ✅ 已完成 | src/Core 分层架构、assets 资源目录 |
| **Texture 类** | ✅ 已完成 | 2D纹理加载、绑定、RAII资源管理 |
| **Mesh 类** | ✅ 已完成 | VAO/VBO/EBO封装、VertexLayout、预定义布局 |
| **Application 类** | ✅ 已完成 | Game Loop封装、窗口管理、输入处理、生命周期虚函数 |
| **RoamingApp 类** | ✅ 已完成 | 继承Application，实现立方体渲染和摄像机控制 |

### 待开发模块

| 模块 | 状态 | 优先级 |
|------|------|--------|
| 地形系统 | ⏳ 待开发 | 核心 |
| 天空盒 | ⏳ 待开发 | 中 |
| 光照系统 | ⏳ 待开发 | 中 |
| 水体系统 | ⏳ 待开发 | 核心 |
| 编辑器UI完善 | ⏳ 待开发 | 中 |
| LOD系统 | ⏳ 待开发 | 加分项 |
| 视锥体剔除 | ⏳ 待开发 | 加分项 |

---

## 🌍 地形规模选择

### 不同规模的性能对比

| 地形规模 | 顶点数 | 三角形数 | 显存占用(网格) | 最低显卡要求 | 建议场景 |
|---------|--------|----------|---------------|-------------|---------|
| **256×256** | 65,536 | ~130K | ~2 MB | 集成显卡 | 学习测试 |
| **512×512** | 262,144 | ~520K | ~8 MB | GTX 750 / 入门独显 | ⭐**推荐** |
| **1024×1024** | 1,048,576 | ~200万 | ~32 MB | GTX 1050+ | 需要LOD优化 |
| **2048×2048** | 4,194,304 | ~800万 | ~128 MB | GTX 1060+ | 必须LOD+剔除 |

### 推荐方案：512×512

**理由**：
1. ✅ **足够展示效果** - 在合理的视野范围内已经很"大"了
2. ✅ **无需 LOD 也能流畅** - 普通笔记本电脑都能稳定 60 FPS
3. ✅ **开发调试快** - 加载快、迭代快
4. ✅ **毕设足够** - 展示"大规模"概念，但不过度复杂
5. ✅ **可扩展** - 后期加 LOD 后可轻松升级到 1024×1024

### 1024×1024 设备要求

如果需要 1024×1024 规模：

| 配置项 | 最低要求 | 推荐配置 |
|-------|---------|---------|
| **显卡** | GTX 1050 / RX 560 | GTX 1060 / RX 580 |
| **显存** | 2GB | 4GB+ |
| **内存** | 8GB | 16GB |
| **CPU** | i5-7400 | i5-9400+ |

---

## 🎨 美术资源推荐

### 1. 高度图 (Heightmap)

| 来源 | 说明 | 链接 |
|------|------|------|
| **Textures.com** | 免费高度图，16位灰度 | https://www.textures.com (搜 "heightmap") |
| **Terrain.party** | ⭐真实地形数据！可下载任意地球位置 | http://terrain.party |
| **L3DT** | 地形生成软件(免费版) | http://www.bundysoft.com/L3DT/ |
| **World Machine** | 专业地形生成(有免费版) | https://www.world-machine.com |

### 2. 地形纹理 (Terrain Textures)

| 来源 | 说明 |
|------|------|
| **Poly Haven** ⭐ | 完全免费的 PBR 纹理，带法线贴图 https://polyhaven.com/textures |
| **ambientCG** | CC0 协议，可商用 https://ambientcg.com |
| **Textures.com** | 免费账户每天15积分 https://www.textures.com |

**需要的纹理类型**：
- 🌿 草地 (Grass) - 绿色植被
- 🪨 岩石 (Rock) - 悬崖、山体
- ❄️ 雪地 (Snow) - 山顶积雪
- 🏜️ 沙地/泥土 (Sand/Dirt) - 可选

**每种纹理需要**：
- Diffuse/Albedo (颜色贴图) - **必需**
- Normal Map (法线贴图) - 推荐，增加细节感
- Roughness (粗糙度) - PBR 可选

**Poly Haven 搜索关键词**：
| 搜索关键词 | 用途 |
|-----------|------|
| `grass ground` | 草地 |
| `rock cliff` 或 `rock boulder` | 岩石 |
| `snow` | 雪地 |
| `brown mud` 或 `forest floor` | 泥土/林地 |

下载时选择：
- 分辨率：**1K** 或 **2K**（不需要 4K，太大了）
- 格式：**JPG**（Diffuse）或 **PNG**

### 3. 天空盒 (Skybox)

| 来源 | 说明 |
|------|------|
| **Poly Haven** ⭐ | 免费 HDR 天空，需要转换为 Cubemap |
| **Humus** | 经典免费天空盒 http://www.humus.name/index.php?page=Textures |
| **OpenGameArt** | 游戏级天空盒 https://opengameart.org (搜 skybox) |

### 4. 水体纹理

| 纹理类型 | 说明 |
|---------|------|
| **DuDv Map** | 用于扭曲反射/折射，制造波动效果 |
| **Normal Map** | 水面法线，制造波光粼粼效果 |

可以从 Google 搜索 "water dudv map" 或 "water normal map" 找到免费素材。

### 5. 推荐资源包结构

```
📁 assets/
├── heightmaps/
│   └── terrain_512.png          # 从 terrain.party 下载
├── textures/
│   ├── terrain/
│   │   ├── grass_diffuse.jpg    # Poly Haven - Ground Grass
│   │   ├── grass_normal.jpg
│   │   ├── rock_diffuse.jpg     # Poly Haven - Rock Boulder
│   │   ├── rock_normal.jpg
│   │   ├── snow_diffuse.jpg     # Poly Haven - Snow
│   │   └── snow_normal.jpg
│   └── water/
│       ├── dudv.png             # 搜索 "water dudv map"
│       └── normal.png           # 搜索 "water normal map"
└── skybox/
    ├── right.jpg                # Humus 天空盒
    ├── left.jpg
    ├── top.jpg
    ├── bottom.jpg
    ├── front.jpg
    └── back.jpg
```

---

## 📅 开发计划与里程碑

### 阶段一：引擎核心重构 (1-2周)

**目标**：建立可扩展的引擎架构

| 任务 | 说明 | 状态 |
|------|------|------|
| 1.1 Application 类 | 封装 Game Loop、窗口管理、输入处理 | ✅ |
| 1.2 ResourceManager | 统一管理 Shader、Texture 资源，避免重复加载 | ⏳ |
| 1.3 Texture 类 | 封装 2D 纹理加载 (stb_image) | ✅ |
| 1.4 Mesh 类 | 抽象 VAO/VBO/EBO 管理 | ✅ |
| 1.5 重构 Camera | 添加 FPS 模式的地面行走限制 | ⏳ |

**产出**：一个干净的引擎框架，为后续开发奠定基础

---

### 阶段二：大规模地形渲染 (2-3周) ⭐核心功能

**目标**：实现高度图地形 + 多材质混合

| 任务 | 说明 | 状态 |
|------|------|------|
| 2.1 HeightmapLoader | 从灰度图生成地形网格顶点 | ⏳ |
| 2.2 TerrainChunk | 地形分块，每块独立 VAO | ⏳ |
| 2.3 TextureArray | 使用 `GL_TEXTURE_2D_ARRAY` 管理多地形材质 | ⏳ |
| 2.4 地形着色器 | Splatmap 多纹理混合、法线计算 | ⏳ |

**关键技术点**：
- **Texture Array**：避免多纹理切换的 Draw Call 开销
- **Splatmap**：使用权重图控制不同材质的混合比例

---

### 阶段三：天空与光照 (1-2周)

**目标**：构建场景光照环境

| 任务 | 说明 | 状态 |
|------|------|------|
| 3.1 Skybox | 立方体贴图天空盒 | ⏳ |
| 3.2 方向光 | 模拟太阳光，可调节方向 | ⏳ |
| 3.3 Blinn-Phong 光照 | 地形的漫反射+镜面反射 | ⏳ |
| 3.4 日夜循环(可选) | 太阳角度随时间变化 | ⏳ |

---

### 阶段四：动态水体 (2-3周) ⭐亮点功能

**目标**：实现具有真实感的水面效果

| 任务 | 说明 | 状态 |
|------|------|------|
| 4.1 水面网格 | 平面网格 + UV 动画 | ⏳ |
| 4.2 反射效果 | 使用裁剪平面 + FBO 渲染反射场景 | ⏳ |
| 4.3 折射效果 | 水下场景渲染到纹理 | ⏳ |
| 4.4 Fresnel 效果 | 视角相关的反射/折射混合 | ⏳ |
| 4.5 波浪动画 | 顶点着色器正弦波叠加 | ⏳ |
| 4.6 岸边泡沫 | 基于深度的泡沫效果 | ⏳ |

---

### 阶段五：编辑器与优化 (1周)

**目标**：完善交互体验与性能调优

| 任务 | 说明 | 状态 |
|------|------|------|
| 5.1 完善 ImGui 编辑器 | 光照/水体/地形参数实时调节 | ⏳ |
| 5.2 性能面板 | FPS、Draw Call、显存占用监控 | ⏳ |
| 5.3 帧率优化 | 确保稳定 60 FPS | ⏳ |
| 5.4 打包与测试 | 生成可发布的 exe | ⏳ |

---

### 【加分项】LOD + 视锥体剔除 (额外1-2周)

| 任务 | 说明 | 状态 |
|------|------|------|
| 6.1 LOD 系统 | 基于摄像机距离动态切换网格精度 | ⏳ |
| 6.2 视锥体剔除 | 只渲染视野内的地形块 (AABB检测) | ⏳ |

---

### 总工期预估

```
┌─────────────────────────────────────────────────────────────┐
│  阶段一：引擎核心重构              1-2 周                    │
├─────────────────────────────────────────────────────────────┤
│  阶段二：地形渲染 (512×512)        2-3 周  ⭐核心            │
├─────────────────────────────────────────────────────────────┤
│  阶段三：天空与光照                1-2 周                    │
├─────────────────────────────────────────────────────────────┤
│  阶段四：动态水体                  2-3 周  ⭐亮点            │
├─────────────────────────────────────────────────────────────┤
│  阶段五：编辑器完善 + 打包          1 周                     │
├─────────────────────────────────────────────────────────────┤
│  【加分项】LOD + 视锥体剔除         额外 1-2 周              │
└─────────────────────────────────────────────────────────────┘

预计总工期：7-11 周（不含加分项）
```

---

## 🛠️ 关键技术实现参考

### 1. 高度图地形生成 (伪代码)

```cpp
// 从高度图生成地形顶点
void generateTerrainMesh(const unsigned char* heightmap, int width, int height) {
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            float y = heightmap[z * width + x] / 255.0f * maxHeight;
            vertices.push_back({x * scale, y, z * scale});
            // 计算UV、法线...
        }
    }
    // 生成索引(三角形网格)
}
```

### 2. Texture Array 地形多材质 (GLSL)

```glsl
// 地形片段着色器
uniform sampler2DArray terrainTextures; // 草地/岩石/雪地
uniform sampler2D splatmap;             // 混合权重图

void main() {
    vec4 blend = texture(splatmap, uv);
    vec3 grass = texture(terrainTextures, vec3(uv * tileScale, 0)).rgb;
    vec3 rock  = texture(terrainTextures, vec3(uv * tileScale, 1)).rgb;
    vec3 snow  = texture(terrainTextures, vec3(uv * tileScale, 2)).rgb;
    
    vec3 finalColor = grass * blend.r + rock * blend.g + snow * blend.b;
}
```

### 3. 水面反射/折射渲染流程

```cpp
// 渲染流程
// 1. 开启裁剪平面，渲染反射场景到 FBO
glEnable(GL_CLIP_DISTANCE0);
reflectionFBO.bind();
camera.invertPitch(); // 翻转摄像机
renderScene(clipPlane = vec4(0, 1, 0, -waterHeight));

// 2. 渲染折射场景到 FBO
refractionFBO.bind();
renderScene(clipPlane = vec4(0, -1, 0, waterHeight));

// 3. 使用反射/折射纹理渲染水面
waterShader.use();
waterShader.setTexture("reflectionTexture", reflectionFBO.colorTexture);
waterShader.setTexture("refractionTexture", refractionFBO.colorTexture);
```

### 4. Fresnel 效果 (GLSL)

```glsl
// 水面着色器中的 Fresnel 计算
vec3 viewVector = normalize(toCameraVector);
float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
refractiveFactor = pow(refractiveFactor, 0.5); // 调整曲线

vec4 finalColor = mix(reflectionColor, refractionColor, refractiveFactor);
```

### 5. 视锥体剔除 (伪代码)

```cpp
// AABB 与视锥体相交检测
bool FrustumCuller::isVisible(const AABB& box) {
    for (int i = 0; i < 6; i++) {
        Plane& plane = frustumPlanes[i];
        
        // 找到 AABB 上离平面最近的点
        glm::vec3 positiveVertex = box.min;
        if (plane.normal.x >= 0) positiveVertex.x = box.max.x;
        if (plane.normal.y >= 0) positiveVertex.y = box.max.y;
        if (plane.normal.z >= 0) positiveVertex.z = box.max.z;
        
        // 如果最近点在平面外侧，则整个 AABB 不可见
        if (plane.distanceToPoint(positiveVertex) < 0) {
            return false;
        }
    }
    return true;
}
```

---

## 🎯 预期成果

产出一个可流畅交互的 Win32 桌面漫游应用程序：
- ✅ 用户可以像操作游戏角色一样在广阔的自然场景中自由漫游
- ✅ 系统具备逼真的多生态地形与动态水效
- ✅ 交互响应灵敏，运行稳定（目标 60FPS）
- ✅ 展现出显著优于普通课程作业的系统完整度与代码规模

---

## 📝 更新日志

| 日期 | 更新内容 |
|------|----------|
| 2026-01-11 | 初始化开发文档，完成系统架构设计和开发计划 |
| 2026-01-11 | 完成项目结构重构：创建 src/Core 分层架构，迁移源码文件，更新 vcxproj 配置 |
| 2026-01-11 | 修复 main.cpp 中文乱码注释，统一使用英文注释 |
| 2026-01-11 | 完成 Texture 类：2D纹理加载、Mipmap、RAII资源管理、着色器纹理采样 |
| 2026-01-12 | 完成 Mesh 类：VAO/VBO/EBO封装、VertexLayout顶点布局、预定义布局(positionColorTexture等)、RAII资源管理 |
| 2026-01-12 | 完成 Application 类：Game Loop封装、窗口管理、输入处理、ImGui集成；创建 RoamingApp 派生类；main.cpp 简化至7行 |
| 2026-01-12 | 重构 Shader 类：添加默认构造函数、load()方法、析构函数、setVec3/setVec4 |

---

*文档版本: v1.4*
*最后更新: 2026-01-12*

