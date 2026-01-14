# PostProcess 模块 - 后处理效果

## 模块概述

PostProcess 模块实现屏幕空间后处理效果，当前包含 SSAO（屏幕空间环境光遮蔽）。后处理在场景渲染完成后，在屏幕空间对图像进行进一步处理。

## 文件列表

| 文件 | 功能 | 说明 |
|------|------|------|
| `SSAO.h/cpp` | SSAO 实现 | 环境光遮蔽效果，增强场景立体感 |

## SSAO 原理

### 什么是 SSAO？

**Screen Space Ambient Occlusion（屏幕空间环境光遮蔽）**：
- 模拟环境光被周围几何体遮挡的效果
- 让角落、缝隙、接触处产生柔和的阴影
- 在屏幕空间计算，性能开销相对较小

### 算法流程

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│  G-Buffer    │ --> │  SSAO Calc   │ --> │  Blur        │
│  Pass        │     │  Pass        │     │  Pass        │
├──────────────┤     ├──────────────┤     ├──────────────┤
│ 渲染场景到    │     │ 在半球内采样  │     │ 4x4盒式模糊   │
│ 位置+法线纹理 │     │ 计算遮蔽因子  │     │ 去除噪点     │
└──────────────┘     └──────────────┘     └──────────────┘
                                                │
                                                ▼
                                         ┌──────────────┐
                                         │  Final Pass  │
                                         │  应用到环境光 │
                                         └──────────────┘
```

### 核心算法

**步骤 1：生成采样核心**
```cpp
// 生成64个随机点在半球内（相对于表面法线）
for (int i = 0; i < 64; i++) {
    vec3 sample = random_in_hemisphere();
    sample = normalize(sample) * random(0, 1);
    
    // 让采样点更集中在原点附近
    float scale = i / 64.0;
    scale = lerp(0.1, 1.0, scale * scale);
    sample *= scale;
    
    kernel.push_back(sample);
}
```

**步骤 2：G-Buffer 渲染**
```cpp
// 渲染场景到两张纹理
gl

FBO.bind();
render_scene_to_gbuffer();
// 输出：RT0 = 视图空间位置，RT1 = 视图空间法线
```

**步骤 3：SSAO 计算**
```glsl
// 对每个像素
vec3 fragPos = texture(gPosition, texCoord).xyz;
vec3 normal = texture(gNormal, texCoord).xyz;

float occlusion = 0.0;
for (int i = 0; i < kernelSize; i++) {
    // 获取采样点位置
    vec3 samplePos = TBN * samples[i];  // 切线空间变换
    samplePos = fragPos + samplePos * radius;
    
    // 投影到屏幕空间
    vec4 offset = projection * vec4(samplePos, 1.0);
    offset.xy /= offset.w;
    offset.xy = offset.xy * 0.5 + 0.5;
    
    // 采样深度并比较
    float sampleDepth = texture(gPosition, offset.xy).z;
    
    // 范围检查，累加遮蔽
    float rangeCheck = smoothstep(0, 1, radius / abs(fragPos.z - sampleDepth));
    occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
}

occlusion = 1.0 - (occlusion / kernelSize);
```

**步骤 4：模糊去噪**
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

**步骤 5：应用到场景**
```glsl
// 地形着色器中
float ssaoFactor = texture(ssaoTexture, screenUV).r;
vec3 ambient = ambientColor * albedo * ssaoFactor;  // 乘以遮蔽因子
```

## 使用示例

```cpp
// 初始化
SSAO ssao;
ssao.init(1920, 1080);

// 渲染循环
void render() {
    // 1. G-Buffer Pass
    ssao.bindGBuffer();
    gbufferShader.use();
    renderScene();  // 输出位置和法线
    
    // 2. SSAO Pass
    ssao.renderSSAO(projection, view);
    
    // 3. Blur Pass
    ssao.renderBlur();
    
    ssao.unbind(windowWidth, windowHeight);
    
    // 4. Final Pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrainShader.use();
    terrainShader.setBool("uSSAOEnabled", true);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, ssao.getSSAOTexture());
    terrainShader.setInt("uSSAOTexture", 6);
    renderScene();
}
```

## 关键参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| **radius** | 0.5 | 采样半径，越大影响范围越广 |
| **bias** | 0.025 | 深度偏移，防止自遮挡伪影 |
| **intensity** | 1.0 | 遮蔽强度，控制暗化程度 |
| **kernelSize** | 32 | 采样点数量，越多越平滑但越慢 |

## 性能优化

| 优化点 | 方法 |
|--------|------|
| 降低分辨率 | SSAO 可以在半分辨率计算 |
| 减少采样 | 32个样本通常已足够 |
| 简化模糊 | 4x4 盒式模糊，不用高斯 |
| 视图空间 | 相比世界空间减少变换 |

## 关键知识点

| 概念 | 说明 |
|------|------|
| **G-Buffer** | Geometry Buffer，存储几何信息的多张纹理 |
| **MRT** | Multiple Render Targets，同时渲染到多张纹理 |
| **半球采样** | 在法线方向的半球内采样（不采样背面） |
| **切线空间** | 以表面法线为Z轴的局部坐标系 |
| **遮蔽因子** | 0=完全遮蔽（黑），1=无遮蔽（白） |

## 着色器接口

### G-Buffer 着色器
```glsl
// 输出
layout (location = 0) out vec4 gPosition;  // 视图空间位置
layout (location = 1) out vec4 gNormal;    // 视图空间法线
```

### SSAO 计算着色器
```glsl
uniform sampler2D uPositionTex;
uniform sampler2D uNormalTex;
uniform sampler2D uNoiseTex;
uniform vec3 uSamples[64];
uniform float uRadius;
uniform float uBias;
```

### 地形着色器应用
```glsl
uniform sampler2D uSSAOTexture;
uniform bool uSSAOEnabled;
uniform float uSSAOIntensity;

// 在环境光计算中
float ssaoFactor = texture(uSSAOTexture, screenUV).r;
vec3 ambient = ambientColor * albedo * mix(1.0, ssaoFactor, uSSAOIntensity);
```
