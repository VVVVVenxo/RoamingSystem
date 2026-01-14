# Terrain 模块 - 地形渲染系统

## 模块概述

Terrain 模块负责从高度图生成可渲染的3D地形，支持视锥体剔除和多级LOD优化。这是项目中最复杂的模块之一，包含了大规模场景渲染的核心优化技术。

## 文件列表

| 文件 | 功能 | 说明 |
|------|------|------|
| `HeightmapLoader.h/cpp` | 高度图加载 | 从灰度PNG加载高度数据 |
| `Terrain.h/cpp` | 地形主接口 | 门面类，封装内部实现 |
| `TerrainChunk.h/cpp` | 地形块 | 单个地形块，含4级LOD网格 |
| `ChunkedTerrain.h/cpp` | 分块管理 | 管理所有块的剔除和LOD |
| `Frustum.h/cpp` | 视锥体剔除 | 检测AABB可见性 |

## 系统架构

```
┌─────────────────────────────────────────────────────────┐
│                      Terrain                             │
│                    (门面接口)                             │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                   ChunkedTerrain                         │
│              (分块管理、LOD选择、视锥体剔除)              │
├─────────────────────────────────────────────────────────┤
│   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐   │
│   │ Chunk 0 │  │ Chunk 1 │  │ Chunk 2 │  │ ...     │   │
│   │ LOD 0-3 │  │ LOD 0-3 │  │ LOD 0-3 │  │         │   │
│   └─────────┘  └─────────┘  └─────────┘  └─────────┘   │
└─────────────────────────────────────────────────────────┘
                          │
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
    ┌──────────┐   ┌──────────┐   ┌──────────┐
    │Heightmap │   │ Frustum  │   │  Shader  │
    │ Loader   │   │ (剔除)   │   │ (渲染)   │
    └──────────┘   └──────────┘   └──────────┘
```

## 核心算法

### 1. 高度图转网格

```cpp
// 高度图坐标 -> 世界坐标
worldX = (heightmapX / heightmapWidth) * terrainSize - terrainSize/2;
worldZ = (heightmapZ / heightmapHeight) * terrainSize - terrainSize/2;
worldY = heightmapValue * maxHeight;

// 法线计算（中心差分法）
float hL = getHeight(x-1, z);  // 左
float hR = getHeight(x+1, z);  // 右
float hD = getHeight(x, z-1);  // 下
float hU = getHeight(x, z+1);  // 上
normal = normalize(vec3(hL - hR, 2.0, hD - hU));
```

### 2. LOD系统

根据块中心到摄像机的距离选择LOD级别：

| LOD | 步长 | 顶点减少 | 适用距离 |
|-----|------|----------|----------|
| 0 | 1 | 0% | < 100m |
| 1 | 2 | 75% | 100-200m |
| 2 | 4 | 93.75% | 200-400m |
| 3 | 8 | 98.4% | > 400m |

### 3. 视锥体剔除

```cpp
// 从VP矩阵提取平面（Gribb/Hartmann方法）
leftPlane   = VP[3] + VP[0];
rightPlane  = VP[3] - VP[0];
bottomPlane = VP[3] + VP[1];
topPlane    = VP[3] - VP[1];
nearPlane   = VP[3] + VP[2];
farPlane    = VP[3] - VP[2];

// AABB可见性测试
for each plane:
    找到AABB距离平面最近的点
    if (点在平面外侧):
        return false;  // 不可见
return true;  // 可见或相交
```

## 使用示例

```cpp
// 创建和生成地形
Terrain terrain;
terrain.generate("assets/heightmaps/terrain.png", 256.0f, 50.0f);

// 渲染循环
void render() {
    glm::mat4 vp = projection * view;
    
    terrainShader.use();
    terrainShader.setMat4("uProjection", projection);
    terrainShader.setMat4("uView", view);
    // ... 设置其他uniforms
    
    terrain.render(terrainShader, camera.Position, vp);
    
    // 获取统计信息
    int visible = terrain.getVisibleChunks();
    int culled = terrain.getCulledChunks();
}
```

## 性能优化效果

典型场景（256x256高度图，16个块）：

| 优化 | 绘制调用 | 三角形数 |
|------|----------|----------|
| 无优化 | 16 | 500K |
| 视锥体剔除 | 4-8 | 125K-250K |
| LOD | 16 | 50K-100K |
| 两者结合 | 4-8 | 12K-50K |

## 关键知识点

| 概念 | 说明 |
|------|------|
| **高度图** | 用灰度图像存储地形高度，黑色=低，白色=高 |
| **LOD** | Level of Detail，远处使用低精度模型 |
| **视锥体剔除** | 跳过摄像机看不到的物体 |
| **AABB** | Axis-Aligned Bounding Box，轴对齐包围盒 |
| **法线贴图** | 用于增加表面细节而不增加几何复杂度 |
| **TBN矩阵** | Tangent-Bitangent-Normal矩阵，用于法线贴图变换 |

## 着色器接口

地形着色器需要接收以下uniform：

```glsl
// 变换矩阵
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// 地形参数
uniform float uMaxHeight;
uniform float uTextureTiling;
uniform vec3 uLightDir;

// 纹理
uniform sampler2D uGrassTexture;
uniform sampler2D uRockTexture;
uniform sampler2D uSnowTexture;

// 法线贴图
uniform sampler2D uGrassNormalMap;
uniform sampler2D uRockNormalMap;
uniform sampler2D uSnowNormalMap;
```
