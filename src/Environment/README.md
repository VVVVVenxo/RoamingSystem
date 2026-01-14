# Environment 模块 - 环境系统

## 模块概述

Environment 模块负责场景的环境渲染，包括天空盒和动态光照系统。这些系统为场景提供背景和光照氛围。

## 文件列表

| 文件 | 功能 | 说明 |
|------|------|------|
| `Skybox.h/cpp` | 天空盒渲染 | 使用立方体贴图渲染无限远背景 |
| `Lighting.h/cpp` | 光照系统 | 日夜循环、动态太阳位置和颜色 |

## 核心功能

### 1. Skybox（天空盒）

**实现原理：**
- 渲染一个以摄像机为中心的大立方体
- 移除视图矩阵的平移分量，使天空盒始终"无限远"
- 使用立方体贴图纹理（6张图片）
- 支持与动态天空颜色混合

**深度测试技巧：**
```glsl
// 顶点着色器
gl_Position = pos.xyww;  // 让深度值为1.0（最远）
```

```cpp
// 渲染时
glDepthFunc(GL_LEQUAL);  // 深度测试改为小于等于
```

**动态颜色混合：**
```glsl
vec3 finalColor = mix(cubemapColor, dynamicSkyColor, blendFactor);
```

### 2. Lighting（光照系统）

**日夜循环模型：**

| 时间 | 太阳位置 | 效果 |
|------|----------|------|
| 6:00 | 地平线（东方） | 日出，暖橙色光 |
| 12:00 | 正上方 | 正午，白色强光 |
| 18:00 | 地平线（西方） | 日落，金橙色光 |
| 0:00 | 地平线下方 | 午夜，微弱蓝光 |

**太阳位置计算：**
```cpp
// 时间转角度（6点=0°，12点=90°，18点=180°）
float angle = (timeOfDay - 6.0f) / 12.0f * PI;

// 球面坐标 -> 笛卡尔坐标
sunDirection = vec3(
    cos(angle),  // X: 东西方向
    sin(angle),  // Y: 高度
    -0.3f        // Z: 略微偏移
);
```

**颜色计算：**
根据太阳高度（Y分量）插值不同时段的颜色：

```cpp
if (sunHeight > 0.3f)        // 白天
    return dayColor;
else if (sunHeight > 0.0f)   // 日出/日落
    return mix(sunsetColor, dayColor, t);
else                         // 夜晚
    return nightColor;
```

## 使用示例

```cpp
// 初始化
Skybox skybox;
skybox.load("assets/skybox", ".png");

Lighting lighting;

// 渲染循环
void render() {
    // 更新光照
    lighting.update(deltaTime);
    
    // 获取动态颜色
    glm::vec3 skyColor = lighting.getSkyColor();
    glm::vec3 sunDir = lighting.getSunDirection();
    glm::vec3 sunColor = lighting.getSunColor();
    
    // 设置背景色
    setClearColor(skyColor);
    
    // 渲染天空盒
    skybox.render(view, projection, skyColor, blendFactor);
    
    // 渲染地形（使用光照参数）
    terrainShader.setVec3("uLightDir", -sunDir);
    terrainShader.setVec3("uLightColor", sunColor);
    terrainShader.setVec3("uAmbientColor", lighting.getAmbientColor());
}
```

## 着色器接口

### Skybox 着色器

```glsl
// 顶点着色器
in vec3 aPos;
out vec3 vTexCoord;

uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    mat4 viewNoTranslation = mat4(mat3(uView));  // 移除平移
    vec4 pos = uProjection * viewNoTranslation * vec4(aPos, 1.0);
    gl_Position = pos.xyww;  // 深度设为1.0
    vTexCoord = aPos;        // 使用位置作为采样坐标
}

// 片段着色器
in vec3 vTexCoord;
uniform samplerCube uSkybox;
uniform vec3 uSkyColor;
uniform float uBlendFactor;

void main() {
    vec4 texColor = texture(uSkybox, vTexCoord);
    vec3 finalColor = mix(texColor.rgb, uSkyColor, uBlendFactor);
    FragColor = vec4(finalColor, 1.0);
}
```

## 关键知识点

| 概念 | 说明 |
|------|------|
| **立方体贴图** | 6个2D纹理组成的立方体，用3D向量采样 |
| **深度测试** | GL_LEQUAL 允许深度=1.0的片段通过 |
| **视图矩阵** | 移除平移使天空盒"无限远" |
| **欧拉角** | 通过角度计算方向向量 |
| **颜色插值** | glm::mix()线性插值实现平滑过渡 |
| **日夜循环** | 基于时间的光照和天空颜色变化 |

## 性能考虑

- 天空盒优先渲染，关闭深度写入
- 立方体贴图使用 CLAMP_TO_EDGE 避免接缝
- 反射纹理可以使用较低分辨率的天空盒
