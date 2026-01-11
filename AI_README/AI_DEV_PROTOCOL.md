# AI 开发协议 (AI Development Protocol)

> ⚠️ **本文档是 AI 辅助开发的强制性规范，所有代码生成必须严格遵守。**

---

## 1. 核心协作原则 (Core Collaboration Rules)

### 🚫 1.1 严禁越界 (No Scope Creep)

- **严格按照 Prompt 要求的范围编写代码**
- 禁止在未告知的情况下引入新的第三方库
- 禁止在未告知的情况下擅自增加与当前任务无关的"优化"或"特性"
  - ❌ 错误示例：让我画个立方体，不要顺手把光照也没问过我就加上了

### ❓ 1.2 确认优先 (Ask Before Assume)

- 遇到需求模糊之处，**必须先询问，禁止脑补**
  - 例如：坐标系是用左手还是右手？纹理是用 PNG 还是 DDS？
- 若发现之前的代码存在严重架构缺陷，需先提出警告和重构方案，**获得批准后再动手**

### 📝 1.3 纯净输出 (Clean Output)

- **代码块中严禁出现 Emoji**
- 解释性文字（Markdown）中可以适度使用 Emoji 以辅助阅读（如 ✅ ❌）
- 源码文件中（`.cpp`, `.h`, `.glsl`）**必须保持纯文本**

---

## 2. C++ 编码规范 (C++ Coding Standards)

### 2.1 版本标准

- 严格遵循 **C++ 17** 标准

### 2.2 命名约定 (Naming Convention)

| 类型 | 规范 | 示例 |
|------|------|------|
| 类名/结构体 | PascalCase | `Camera`, `TextureManager` |
| 函数/方法 | camelCase | `processInput`, `loadShader` |
| 变量/参数 | camelCase | `deltaTime`, `windowWidth` |
| 成员变量 | **m_ 前缀** | `m_position`, `m_viewMatrix` |
| 宏/常量 | UPPER_SNAKE_CASE | `MAX_LIGHTS`, `SCR_WIDTH` |

> ⚠️ **成员变量必须带 `m_` 前缀**，这对 OpenGL 数学运算极其重要，防止重名混淆。

### 2.3 代码风格

| 项目 | 规范 |
|------|------|
| 大括号 | 换行风格 (Allman) 或 同行风格 (K&R)，需保持全项目一致（默认使用 VS 默认风格） |
| 指针/引用 | 符号靠左：`Camera* camera`, `const std::string& path` |

### 2.4 注释规范

| 规则 | 说明 |
|------|------|
| 禁止废话 | ❌ `int a = 1; // 声明一个变量a等于1` |
| 语言 | 源码注释尽量使用简单英文，或确保文件保存为 **UTF-8 with BOM** 格式 |
| TODO 标记 | 未完成的功能必须标记 `// TODO: [说明]` |

---

## 3. OpenGL/GLSL 开发规范 (Graphics Specifics)

### 3.1 着色器 (Shader)

| 规则 | 说明 |
|------|------|
| 版本声明 | 统一使用 `#version 330 core` 或 `#version 450 core`（需与 C++ 代码匹配） |
| Uniform 变量 | 使用 `u` 前缀：`uniform mat4 uModel;` |
| 顶点属性 | 使用 `a` 前缀：`layout (location=0) in vec3 aPos;` |

**示例：**

```glsl
#version 450 core

// Vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// Uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    // Matrix order: Projection * View * Model
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
```

### 3.2 资源管理 (RAII)

- **尽量避免**在 main 循环中进行 `new`/`malloc`
- OpenGL 对象（VAO, VBO, Texture ID）应当封装在类中
- 在**析构函数**中调用 `glDelete*` 进行清理

### 3.3 数学运算

- 统一使用 **GLM** 库
- 矩阵运算顺序必须在注释中明确：`Projection * View * Model`

---

## 4. 交互与修正 (Interaction & Refinement)

### 4.1 增量修改 (Incremental Changes)

- 如果只修改了 5 行代码，**不要重新输出整个 500 行的文件**
- 使用"查找-替换"的格式，或者只输出修改的函数块
- 注明行号或上下文

**示例格式：**

```
在 main.cpp 第 45 行，将：
    camera.ProcessKeyboard(FORWARD, deltaTime);
修改为：
    camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
```

### 4.2 错误处理 (Error Handling)

| 场景 | 要求 |
|------|------|
| 文件读取（Shader, Texture） | 必须包含 `try-catch` 或 `if (!data) std::cerr` 检查 |
| GL 调用关键处 | 建议插入 `glGetError()` 检查（调试阶段） |

---

## 5. 快速检查清单 (Quick Checklist)

在提交代码前，确认以下事项：

- [ ] 是否严格按照 Prompt 要求，没有额外添加功能？
- [ ] 成员变量是否都带 `m_` 前缀？
- [ ] Shader 中 Uniform 是否带 `u` 前缀，顶点属性是否带 `a` 前缀？
- [ ] 源码中是否没有 Emoji？
- [ ] 是否使用增量修改而非全文输出？
- [ ] 文件读取是否有错误处理？

---

*文档版本: v1.0*  
*最后更新: 2026-01-11*
