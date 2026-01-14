# Core 模块 - 核心引擎组件

## 模块概述

Core 模块包含渲染引擎的基础组件，提供摄像机控制、着色器管理、纹理加载和网格渲染等核心功能。这些组件被其他所有模块依赖。

## 文件列表

| 文件 | 功能 | 说明 |
|------|------|------|
| `Camera.h` | 第一人称摄像机 | 管理视角移动和旋转，生成视图矩阵 |
| `Shader.h/cpp` | 着色器程序管理 | 加载、编译、链接GLSL着色器，设置uniform |
| `Texture.h/cpp` | 2D纹理管理 | 从图片加载纹理，绑定到纹理单元 |
| `Cubemap.h/cpp` | 立方体贴图 | 加载天空盒纹理（6张图片） |
| `Mesh.h/cpp` | 网格管理 | 封装VAO/VBO/EBO，管理顶点数据 |
| `stb_image_impl.cpp` | stb_image实现 | 图片加载库的实现文件 |

## 核心类说明

### Camera（摄像机）

```cpp
// 创建摄像机，位于(0, 30, 50)位置
Camera camera(glm::vec3(0.0f, 30.0f, 50.0f));

// 获取视图矩阵
glm::mat4 view = camera.GetViewMatrix();

// 处理键盘输入（WASD移动）
camera.ProcessKeyboard(FORWARD, deltaTime);

// 处理鼠标输入（视角旋转）
camera.ProcessMouseMovement(xoffset, yoffset);
```

**关键概念**：
- **欧拉角**：用Yaw（偏航）和Pitch（俯仰）表示视角方向
- **视图矩阵**：通过`glm::lookAt()`生成，用于MVP变换

### Shader（着色器）

```cpp
Shader shader;
shader.load("shaders/terrain.vert", "shaders/terrain.frag");

// 使用着色器
shader.use();

// 设置uniform变量
shader.setMat4("uProjection", projection);
shader.setVec3("uLightDir", lightDir);
shader.setFloat("uTime", time);
shader.setInt("uTexture", 0);  // 纹理单元0
```

**着色器编译流程**：
1. 读取.vert和.frag源文件
2. 创建着色器对象并编译
3. 创建程序对象并链接
4. 删除着色器对象（已链接）

### Texture（2D纹理）

```cpp
Texture texture;
texture.load("assets/textures/grass.png");

// 绑定到纹理单元0
texture.bind(0);

// 在着色器中使用
shader.setInt("uGrassTexture", 0);
```

**注意**：OpenGL纹理坐标原点在左下角，而图片原点在左上角，加载时默认会垂直翻转。

### Mesh（网格）

```cpp
// 定义顶点数据（位置+法线+纹理坐标）
float vertices[] = { ... };

Mesh mesh;
mesh.setVertices(vertices, sizeof(vertices), VertexLayout::positionNormalTexture());

// 可选：设置索引
unsigned int indices[] = { 0, 1, 2, ... };
mesh.setIndices(indices, indexCount);

// 绘制
mesh.draw(GL_TRIANGLES);
```

**顶点布局**：
- `positionOnly()`: 仅位置 (12字节/顶点)
- `positionNormalTexture()`: 位置+法线+UV (32字节/顶点)
- `positionNormalTextureTangent()`: 带切线 (44字节/顶点)

## 渲染流程

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Camera     │───▶│  View Matrix│───▶│  Shader     │
│  (位置/角度) │    │  (视图变换)  │    │  (MVP变换)  │
└─────────────┘    └─────────────┘    └─────────────┘
                                            │
                                            ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Texture    │───▶│  着色器采样  │───▶│  Mesh       │
│  (纹理绑定)  │    │             │    │  (顶点绘制) │
└─────────────┘    └─────────────┘    └─────────────┘
```

## 相关OpenGL知识

| 概念 | 说明 |
|------|------|
| **VAO** | 顶点数组对象，存储顶点属性配置 |
| **VBO** | 顶点缓冲对象，存储顶点数据 |
| **EBO** | 索引缓冲对象，存储索引数据 |
| **Uniform** | 着色器全局变量，从CPU传递到GPU |
| **纹理单元** | GPU中用于采样纹理的槽位（0-15） |
| **Mipmap** | 多级渐远纹理，优化远距离采样 |

## 依赖关系

```
stb_image (图片加载)
    │
    ├── Texture
    │
    └── Cubemap

GLAD (OpenGL函数加载)
    │
    ├── Shader
    ├── Texture
    ├── Cubemap
    └── Mesh

GLM (数学库)
    │
    ├── Camera
    └── Shader (uniform设置)
```
