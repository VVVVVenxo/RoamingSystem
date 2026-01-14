# Editor 模块 - 场景编辑器

## 模块概述

Editor 模块提供场景参数的保存和加载功能，使用 INI 格式文件持久化所有可调参数。这样用户可以保存喜欢的场景配置，下次启动时自动恢复。

## 文件列表

| 文件 | 功能 | 说明 |
|------|------|------|
| `SceneSettings.h` | 场景设置结构 | 包含所有可配置参数 |
| `SceneSettings.cpp` | 序列化实现 | INI 格式读写 |

## 功能说明

### SceneSettings 结构

包含以下类别的参数：

| 类别 | 参数 |
|------|------|
| **Terrain** | 纹理平铺、混合阈值 |
| **Water** | 水面高度、波浪参数、颜色 |
| **Lighting** | 时间、日夜循环速度 |
| **Fog** | 雾效开关、密度 |
| **SSAO** | SSAO 开关、半径、偏移、强度 |
| **Camera** | 摄像机位置、速度、地面行走模式 |
| **Display** | 线框模式、参考立方体 |

### INI 文件格式

```ini
[Terrain]
textureTiling=32.000000
grassMaxHeight=0.350000
rockMaxHeight=0.700000
slopeThreshold=0.400000
useTerrainTextures=1

[Water]
waterHeight=10.000000
waveSpeed=0.030000
waveStrength=0.020000
...

[Lighting]
timeOfDay=12.000000
daySpeed=0.100000
autoAdvance=0

[Fog]
enableFog=1
fogDensity=0.003000

[SSAO]
enableSSAO=1
ssaoRadius=0.500000
ssaoBias=0.025000
ssaoIntensity=1.000000
ssaoKernelSize=32

[Camera]
cameraPosX=0.000000
...
```

## 使用示例

```cpp
// 保存当前场景设置
SceneSettings settings;
settings.textureTiling = 64.0f;
settings.waterHeight = 15.0f;
settings.timeOfDay = 18.0f;  // 日落
// ... 填充其他参数

SceneSettingsManager::save(settings, "settings.ini");

// 加载设置
SceneSettings loadedSettings;
if (SceneSettingsManager::load(loadedSettings, "settings.ini"))
{
    // 应用到场景
    terrain.setTextureTiling(loadedSettings.textureTiling);
    water.setHeight(loadedSettings.waterHeight);
    lighting.m_timeOfDay = loadedSettings.timeOfDay;
    // ...
}
```

## 实现细节

### 保存流程

1. 从场景中收集当前参数值
2. 打开 `settings.ini` 文件
3. 按分段（Section）写入 key=value 对
4. 关闭文件

### 加载流程

1. 打开 `settings.ini` 文件
2. 逐行读取
3. 跳过空行和分段标记 `[Section]`
4. 解析 `key=value` 格式
5. 根据 key 设置对应的 settings 字段
6. 如果文件不存在或某项缺失，使用默认值

### 类型转换

```cpp
// 布尔值：1/0
settings.enableFog = (std::stoi(value) != 0);

// 浮点数
settings.fogDensity = std::stof(value);

// 整数
settings.ssaoKernelSize = std::stoi(value);

// Vec3：分3个字段存储
waterColorR, waterColorG, waterColorB
```

## 扩展可能

如果需要更复杂的配置，可以考虑：
- 使用 JSON 格式（需引入第三方库如 nlohmann/json）
- 使用 YAML 格式（更易读）
- 支持多个配置文件切换
- 添加预设配置（Dawn.ini, Noon.ini, Dusk.ini）
