# 图形学术语中英对照表

> 本术语表专为翻译 *"Terrain Rendering Using GPU-Based Geometry Clipmaps"* 论文而整理。
> 翻译时请严格遵循此表，确保全文术语一致性。

---

## 一、核心概念

| 英文 | 中文翻译 | 首次出现 | 备注 |
|------|----------|---------|------|
| Geometry Clipmaps | 几何裁剪图 | 标题 | 本文核心概念，首次出现需注释 |
| Clipmap | 裁剪图 | 2.1 | Geometry Clipmaps的简称 |
| Terrain | 地形 | 2.1 | |
| Heightfield / Heightmap | 高度场 / 高度图 | 2.1 | 可互换使用 |
| Level of Detail (LOD) | 细节层次 | 2.2 | 保留英文缩写LOD |
| Real-time Rendering | 实时渲染 | 2.1 | |

---

## 二、几何与网格

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Mesh | 网格 | |
| Grid | 网格/栅格 | 规则网格时译"栅格" |
| Nested Grids | 嵌套网格 | |
| Regular Grid | 规则网格 | |
| Vertex | 顶点 | 复数 Vertices |
| Triangle | 三角形 | |
| Polygon | 多边形 | |
| Primitive | 图元 | |
| Index / Indices | 索引 | |
| Footprint | 覆盖范围 | 指网格在地形上的覆盖区域 |

---

## 三、空间与坐标

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| World Space | 世界空间 | |
| View Space / Eye Space | 视图空间/观察空间 | |
| Clip Space | 裁剪空间 | |
| Screen Space | 屏幕空间 | |
| Texture Coordinates | 纹理坐标 | 常用 (u, v) 表示 |
| Toroidal | 环形的 | 环形数据访问 |
| Toroidal Access | 环形访问 | 数据结构技术 |

---

## 四、视图与剔除

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| View Frustum | 视锥体 | |
| Frustum Culling | 视锥体剔除 | |
| Culling | 剔除 | |
| Clipping | 裁剪 | 与Culling区分 |
| Clip Plane | 裁剪平面 | |
| Bounding Box | 包围盒 | |
| AABB | 轴对齐包围盒 | Axis-Aligned Bounding Box |
| Occlusion | 遮挡 | |
| Visible / Visibility | 可见/可见性 | |

---

## 五、渲染管线

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Rendering Pipeline | 渲染管线 | |
| Vertex Shader | 顶点着色器 | |
| Fragment Shader | 片段着色器 | 也称"像素着色器" |
| Pixel Shader | 像素着色器 | 同 Fragment Shader |
| Geometry Shader | 几何着色器 | |
| Shader Program | 着色器程序 | |
| Pass | 渲染通道 | 如"第一遍渲染" |
| Draw Call | 绘制调用 | |

---

## 六、纹理与采样

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Texture | 纹理 | 学术论文用"纹理"，非"贴图" |
| Texture Mapping | 纹理映射 | |
| Sampler | 采样器 | |
| Sampling | 采样 | |
| Texel | 纹素 | Texture Element |
| Mipmap | 多级渐远纹理 | 保留英文Mipmap也可 |
| Texture Splatting | 纹理混合 | 也译"纹理喷溅" |
| Blending | 混合 | |
| Normal Map | 法线贴图 | |
| Height Map | 高度图 | |

---

## 七、GPU与内存

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| GPU | 图形处理器 | 可直接用GPU |
| Vertex Texture | 顶点纹理 | GPU特性 |
| Texture Fetch | 纹理获取 | |
| Buffer | 缓冲区 | |
| Vertex Buffer | 顶点缓冲区 | |
| Index Buffer | 索引缓冲区 | |
| Frame Buffer | 帧缓冲区 | |
| Render Target | 渲染目标 | |
| VRAM | 显存 | Video RAM |
| Bandwidth | 带宽 | |
| Cache | 缓存 | |

---

## 八、数学与计算

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Matrix | 矩阵 | 复数 Matrices |
| Vector | 向量 | |
| Normal / Normal Vector | 法线/法向量 | |
| Tangent | 切线 | |
| Bitangent | 副切线 | |
| Dot Product | 点积 | |
| Cross Product | 叉积 | |
| Interpolation | 插值 | |
| Bilinear Interpolation | 双线性插值 | |
| Central Differencing | 中心差分 | 法线计算方法 |
| Normalize | 归一化 | |

---

## 九、LOD相关

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Level of Detail | 细节层次 | |
| Coarse / Coarser | 粗糙/更粗糙 | 指低精度LOD |
| Fine / Finer | 精细/更精细 | 指高精度LOD |
| Resolution | 分辨率 | |
| Grid Spacing | 网格间距 | |
| Transition | 过渡 | LOD间的过渡 |
| Blend Region | 混合区域 | |
| Morphing | 变形 | 顶点平滑过渡 |
| Geomorphing | 几何变形 | |
| Popping | 跳变/突变 | LOD切换时的视觉瑕疵 |

---

## 十、更新与压缩

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Update | 更新 | |
| Incremental Update | 增量更新 | |
| Streaming | 流式加载 | |
| Compression | 压缩 | |
| Decompression | 解压缩 | |
| Residual | 残差 | 压缩技术术语 |

---

## 十一、性能相关

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Frame Rate | 帧率 | |
| FPS | 帧每秒 | Frames Per Second |
| Performance | 性能 | |
| Bottleneck | 瓶颈 | |
| Overhead | 开销 | |
| Throughput | 吞吐量 | |
| Latency | 延迟 | |
| Optimization | 优化 | |

---

## 十二、其他常用术语

| 英文 | 中文翻译 | 备注 |
|------|----------|------|
| Algorithm | 算法 | |
| Implementation | 实现 | |
| Pipeline | 管线/流水线 | |
| Architecture | 架构 | |
| Framework | 框架 | |
| Module | 模块 | |
| Parameter | 参数 | |
| Configuration | 配置 | |
| Threshold | 阈值 | |
| Coefficient | 系数 | |

---

## 使用说明

### 翻译原则

1. **术语一致性**：同一术语全文使用相同译法
2. **首次标注**：核心术语首次出现时标注英文原文，如：几何裁剪图（Geometry Clipmaps）
3. **缩写保留**：LOD、GPU、AABB等缩写可直接使用
4. **括号补充**：不常见术语可用括号补充，如：环形访问（Toroidal Access）

### 示例

```
原文：The geometry clipmap structure caches terrain in a set of 
      nested regular grids centered about the viewer.

译文：几何裁剪图（Geometry Clipmaps）结构将地形数据缓存在一组
      以观察者为中心的嵌套规则网格中。
```

### 争议术语处理

| 术语 | 可选译法 | 本文采用 | 理由 |
|------|---------|---------|------|
| Texture | 纹理/贴图 | 纹理 | 学术论文惯例 |
| Clipmap | 裁剪图/剪贴图 | 裁剪图 | 更准确 |
| Splatting | 喷溅/混合 | 混合 | 更易理解 |

---

## 参考资料

1. 《计算机图形学（第四版）》- 清华大学出版社 术语表
2. OpenGL官方规范中文翻译
3. Real-Time Rendering 中文版术语
4. 浙江大学CAD&CG国家重点实验室术语规范

---

*版本: v1.0*
*创建日期: 2026-01-14*
*适用论文: GPU Gems 2, Chapter 2*
