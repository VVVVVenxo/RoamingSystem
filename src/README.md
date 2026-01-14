# Source Code 目录

## 概述

src/ 目录包含整个渲染引擎的所有源代码，采用模块化分层架构设计。

## 文件结构

```
src/
├── main.cpp                    # 程序入口（7行）
├── Application.h/cpp           # 应用框架基类（Game Loop、GLFW、ImGui）
├── RoamingApp.h/cpp            # 主应用类（渲染逻辑）
├── Core/                       # 核心引擎组件
├── Terrain/                    # 地形渲染系统
├── Water/                      # 水体渲染系统
├── Environment/                # 环境系统（天空盒、光照）
├── PostProcess/                # 后处理效果（SSAO）
└── Editor/                     # 场景编辑器（设置保存/加载）
```

## 模块简介

### Core 模块
核心引擎组件，提供基础渲染功能：
- **Camera**: 第一人称摄像机
- **Shader**: 着色器程序管理
- **Texture**: 2D 纹理加载
- **Cubemap**: 立方体贴图（天空盒）
- **Mesh**: VAO/VBO/EBO 封装

### Terrain 模块
地形渲染系统，支持大规模场景：
- **HeightmapLoader**: 从灰度图加载高度数据
- **Terrain**: 地形主接口
- **ChunkedTerrain**: 分块管理
- **TerrainChunk**: 单个地形块（4级LOD）
- **Frustum**: 视锥体剔除

### Water 模块
逼真的水体渲染：
- **Water**: 水面网格与渲染
- **WaterFramebuffers**: 反射/折射 FBO

### Environment 模块
环境效果：
- **Skybox**: 天空盒渲染
- **Lighting**: 日夜循环光照系统

### PostProcess 模块
屏幕空间后处理：
- **SSAO**: 环境光遮蔽

### Editor 模块
场景编辑与持久化：
- **SceneSettings**: 参数序列化（INI格式）

## 代码统计

| 模块 | 文件数 | 代码行数 |
|------|--------|----------|
| Core | 11 | ~900 |
| Terrain | 10 | ~800 |
| Water | 4 | ~400 |
| Environment | 4 | ~350 |
| PostProcess | 2 | ~300 |
| Editor | 2 | ~180 |
| Application | 6 | ~700 |
| **总计** | **39** | **~3,630** |

## 主要设计模式

| 模式 | 应用 |
|------|------|
| **门面模式** | Terrain 封装 ChunkedTerrain 复杂性 |
| **RAII** | 所有资源类自动管理生命周期 |
| **移动语义** | Mesh、Texture等禁用拷贝，支持移动 |
| **模板方法** | Application定义框架，RoamingApp实现细节 |

## 编译配置

项目使用 Visual Studio 2022，C++17 标准。

**关键编译选项：**
- `/utf-8` - 支持 UTF-8 源文件（如需中文注释）
- `/std:c++17` - C++17 标准
- OpenGL 4.5 Core Profile

## 依赖关系

```
RoamingApp
    ├── Application (GLFW + ImGui)
    ├── Core (Camera, Shader, Texture, Mesh)
    ├── Terrain (HeightmapLoader, ChunkedTerrain, Frustum)
    ├── Water (Water, WaterFramebuffers)
    ├── Environment (Skybox, Lighting)
    ├── PostProcess (SSAO)
    └── Editor (SceneSettings)
```
