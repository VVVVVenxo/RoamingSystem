# Shaders 目录 - GLSL着色器

## 概述

本目录包含所有GLSL着色器文件。着色器是运行在GPU上的小程序，负责顶点变换和像素着色。

## 文件列表

| 着色器 | 功能 | 说明 |
|--------|------|------|
| `terrain.vert/frag` | 地形渲染 | 多纹理混合、法线贴图、光照、雾效、SSAO |
| `water.vert/frag` | 水面渲染 | 反射/折射、Fresnel、DuDv波浪、泡沫 |
| `skybox.vert/frag` | 天空盒 | 立方体贴图采样、动态颜色混合 |
| `gbuffer.vert/frag` | G-Buffer | 输出位置和法线（SSAO用） |
| `ssao.vert/frag` | SSAO计算 | 半球采样计算遮蔽因子 |
| `ssao_blur.frag` | SSAO模糊 | 4x4盒式模糊去噪 |
| `basic.vert/frag` | 基础渲染 | 简单的颜色+纹理着色器 |

## 着色器详解

### 1. terrain.vert/frag - 地形渲染

**顶点着色器输入**：
```glsl
layout (location = 0) in vec3 aPos;       // 位置
layout (location = 1) in vec3 aNormal;    // 法线
layout (location = 2) in vec2 aTexCoord;  // 纹理坐标
layout (location = 3) in vec3 aTangent;   // 切线（法线贴图用）
```

**片段着色器功能**：
1. **多纹理混合** - 根据高度和坡度混合草地/岩石/雪地
2. **法线贴图** - TBN矩阵变换切线空间法线到世界空间
3. **动态光照** - 方向光（太阳）+ 环境光
4. **雾效** - 指数雾公式
5. **SSAO** - 应用环境光遮蔽因子

**关键算法**：
```glsl
// 坡度计算
float slope = 1.0 - dot(normal, vec3(0, 1, 0));

// 纹理权重
grassWeight = smoothstep(...) * smoothstep(...);
rockWeight = ...;
snowWeight = ...;

// 法线贴图
vec3 tangentNormal = texture(normalMap, uv).rgb * 2.0 - 1.0;
vec3 worldNormal = normalize(TBN * tangentNormal);

// 光照
vec3 ambient = ambientColor * albedo * ssaoFactor;
vec3 diffuse = lightColor * NdotL * albedo * lightIntensity;
```

### 2. water.vert/frag - 水面渲染

**片段着色器功能**：
1. **DuDv扭曲** - 使用DuDv贴图扭曲反射/折射纹理坐标
2. **法线贴图** - 水面波浪细节
3. **Fresnel效果** - 视角相关的反射/折射混合
4. **高光** - 镜面反射高光
5. **岸边泡沫** - 基于深度的泡沫效果

**关键算法**：
```glsl
// DuDv扭曲
vec2 distortion = (texture(dudvMap, texCoord + time).rg * 2.0 - 1.0) * waveStrength;
reflectTexCoord += distortion;

// Fresnel
float fresnel = dot(normalize(toCamera), vec3(0,1,0));
fresnel = pow(fresnel, 0.5);
color = mix(reflection, refraction, fresnel);

// 岸边泡沫
float waterDepth = terrainDepth - waterSurfaceDepth;
float foamFactor = 1.0 - clamp(waterDepth / foamThreshold, 0, 1);
```

### 3. skybox.vert/frag - 天空盒

**顶点着色器技巧**：
```glsl
// 移除视图矩阵的平移
mat4 viewNoTranslation = mat4(mat3(uView));

// 深度设为1.0（最远）
vec4 pos = uProjection * viewNoTranslation * vec4(aPos, 1.0);
gl_Position = pos.xyww;  // z = w，透视除法后z/w = 1.0
```

**片段着色器**：
```glsl
// 采样立方体贴图
vec4 texColor = texture(uSkybox, vTexCoord);

// 与动态天空颜色混合（日夜循环）
vec3 finalColor = mix(texColor.rgb, uSkyColor, uBlendFactor);
```

### 4. G-Buffer 着色器

**用途**：为SSAO提供几何信息

```glsl
// gbuffer.frag输出
layout (location = 0) out vec4 gPosition;  // 视图空间位置
layout (location = 1) out vec4 gNormal;    // 视图空间法线
```

### 5. SSAO 着色器

**ssao.frag 核心算法**：
```glsl
// 对每个片段
vec3 fragPos = texture(gPosition, texCoord).xyz;
vec3 normal = texture(gNormal, texCoord).xyz;

// 构建切线空间（用噪声旋转采样）
vec3 randomVec = texture(noiseTex, texCoord * noiseScale).xyz;
mat3 TBN = constructTBN(normal, randomVec);

// 半球采样
float occlusion = 0.0;
for (int i = 0; i < kernelSize; i++) {
    vec3 samplePos = TBN * samples[i];
    samplePos = fragPos + samplePos * radius;
    
    // 投影到屏幕空间查询深度
    vec4 offset = projection * vec4(samplePos, 1.0);
    offset.xy = offset.xy / offset.w * 0.5 + 0.5;
    
    float sampleDepth = texture(gPosition, offset.xy).z;
    
    // 范围检查并累加遮蔽
    occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);
}

occlusion = 1.0 - (occlusion / kernelSize);
```

**ssao_blur.frag**：
```glsl
// 简单4x4盒式模糊
float result = 0.0;
for (int x = -2; x < 2; x++) {
    for (int y = -2; y < 2; y++) {
        result += texture(ssaoInput, texCoord + offset).r;
    }
}
result /= 16.0;
```

## Uniform 变量规范

本项目统一使用 `u` 前缀表示 uniform 变量：

| 前缀 | 用途 | 示例 |
|------|------|------|
| `u` | Uniform变量 | `uProjection`, `uLightDir` |
| `a` | 顶点属性 | `aPos`, `aNormal` |
| `v` | Varying（顶点→片段）| `vTexCoord`, `vNormal` |

## 着色器编译

着色器在运行时动态编译：

```cpp
Shader shader;
shader.load("shaders/terrain.vert", "shaders/terrain.frag");
```

编译失败时会在控制台输出详细错误信息。

## 性能考虑

| 着色器 | 优化策略 |
|--------|----------|
| terrain | LOD降低远处顶点数，视锥体剔除 |
| water | 反射使用低分辨率FBO (320x180) |
| ssao | 降低采样数（32→16），半分辨率计算 |

## GLSL 版本

所有着色器使用 **GLSL 4.50 Core**：
```glsl
#version 450 core
```

兼容 OpenGL 4.5+。
