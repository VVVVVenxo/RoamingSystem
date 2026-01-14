# Water 模块 - 水体渲染系统

## 模块概述

Water 模块实现了逼真的水面渲染效果，包括反射、折射、Fresnel效果、波浪动画和岸边泡沫。这是实现"游戏级"视觉效果的关键模块。

## 文件列表

| 文件 | 功能 | 说明 |
|------|------|------|
| `Water.h/cpp` | 水面渲染 | 水面网格、着色器、纹理管理 |
| `WaterFramebuffers.h/cpp` | FBO管理 | 反射和折射的帧缓冲 |

## 渲染原理

### 1. 反射与折射

水面效果需要两次额外的场景渲染：

```
┌─────────────────────────────────────────────────────────┐
│                     正常场景                             │
│    ┌─────────┐                                          │
│    │ 天空盒  │                                          │
│    └─────────┘                                          │
│                         ← 摄像机                         │
│    ═══════════════════════════════════  ← 水面          │
│    ┌─────────┐                                          │
│    │  地形   │                                          │
│    └─────────┘                                          │
└─────────────────────────────────────────────────────────┘

┌──────────────────┐     ┌──────────────────┐
│   反射渲染        │     │   折射渲染        │
│                  │     │                  │
│  翻转摄像机      │     │  正常摄像机       │
│  只渲染水面以上   │     │  只渲染水面以下   │
│                  │     │                  │
│  结果：倒影      │     │  结果：水下场景   │
└──────────────────┘     └──────────────────┘
```

### 2. Fresnel效果

```
垂直观看水面 → 更透明（看到折射）
平视水面边缘 → 更反射（看到倒影）

fresnel = dot(viewDir, normal)
finalColor = mix(reflection, refraction, fresnel)
```

### 3. DuDv波浪

DuDv贴图存储纹理坐标偏移量：
- R通道 = X方向偏移
- G通道 = Y方向偏移

```glsl
vec2 distortion = texture(dudvMap, texCoord + time).rg * 2.0 - 1.0;
reflectCoord += distortion * waveStrength;
refractCoord += distortion * waveStrength;
```

### 4. 岸边泡沫

使用深度差计算水深：

```glsl
float waterDepth = terrainDepth - waterSurfaceDepth;
float foamFactor = 1.0 - clamp(waterDepth / foamThreshold, 0, 1);
// waterDepth小 → foamFactor大 → 显示泡沫
```

## 渲染流程

```
1. 设置裁剪平面
   ↓
2. 绑定反射FBO
   ├─ 翻转摄像机（Y轴镜像）
   ├─ 设置裁剪平面（水面以上）
   └─ 渲染场景
   ↓
3. 绑定折射FBO
   ├─ 正常摄像机
   ├─ 设置裁剪平面（水面以下）
   └─ 渲染场景（含深度）
   ↓
4. 解绑FBO
   ↓
5. 正常渲染场景
   ↓
6. 渲染水面
   ├─ 绑定反射纹理
   ├─ 绑定折射纹理
   ├─ 绑定深度纹理
   ├─ 绑定DuDv贴图
   └─ 绘制水面四边形
```

## 使用示例

```cpp
// 初始化
Water water;
water.init(256.0f, 10.0f);  // 大小256，高度10

WaterFramebuffers waterFBOs;
waterFBOs.init(320, 180, 1280, 720);

// 渲染循环
void render() {
    // 1. 反射Pass
    waterFBOs.bindReflectionFBO();
    // ... 用翻转摄像机渲染
    
    // 2. 折射Pass
    waterFBOs.bindRefractionFBO();
    // ... 正常渲染
    
    // 3. 正常渲染
    waterFBOs.unbind(windowWidth, windowHeight);
    renderScene();
    
    // 4. 渲染水面
    water.render(view, projection, cameraPos, lightDir,
                 lightColor, lightIntensity, time,
                 waterFBOs.getReflectionTexture(),
                 waterFBOs.getRefractionTexture(),
                 waterFBOs.getRefractionDepthTexture(),
                 fogColor, fogDensity, fogEnabled);
}
```

## 着色器接口

```glsl
// 顶点着色器输出
out vec4 vClipSpace;      // 裁剪空间坐标（用于纹理坐标计算）
out vec2 vTexCoord;       // 纹理坐标
out vec3 vToCamera;       // 指向摄像机的向量
out vec3 vWorldPos;       // 世界坐标

// 片段着色器uniform
uniform sampler2D uReflectionTexture;
uniform sampler2D uRefractionTexture;
uniform sampler2D uDudvMap;
uniform sampler2D uNormalMap;
uniform sampler2D uDepthMap;
uniform float uTime;
uniform float uWaveStrength;
uniform vec3 uWaterColor;
```

## 性能考虑

| 优化点 | 方法 |
|--------|------|
| 反射分辨率 | 使用低分辨率（320x180），因为会被扭曲 |
| 裁剪平面 | 只渲染水面相关的物体 |
| LOD | 反射/折射渲染时可使用更低LOD |

## 关键知识点

| 概念 | 说明 |
|------|------|
| **FBO** | 帧缓冲对象，允许渲染到纹理 |
| **裁剪平面** | gl_ClipDistance裁剪水面上/下的像素 |
| **Fresnel** | 菲涅尔效应，视角影响反射/折射比例 |
| **DuDv贴图** | 存储纹理坐标偏移的特殊纹理 |
| **深度纹理** | 存储场景深度，用于计算水深 |
