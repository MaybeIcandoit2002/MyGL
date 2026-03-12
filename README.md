# MyGL

# MyGL

基于 OpenGL + Chipmunk2D 的二维物理实验沙盒（本科毕业设计项目原型）。

A 2D physics sandbox prototype for teaching/learning mechanics, built with OpenGL and Chipmunk2D.

## 项目简介 / Overview

该项目当前重点是：

- 用 C++14 搭建可运行的 2D 渲染与物理核心；
- 通过 ImGui 做运行时调试与对象编辑；
- 支持从模板创建对象、编辑参数、保存/加载场景。

当前是“可实验、可迭代”的工程状态，不是生产级引擎。

## 当前功能进度（按代码实际）

### 1) 渲染系统（已实现）

- 自定义渲染封装：`Renderer`、`VertexBuffer`、`IndexBuffer`、`VertexArray`、`UniformBuffer`、`ShaderProgram`
- 实例化绘制（`glDrawElementsInstanced`）
- 基础纹理槽管理（当前最多 8 个纹理槽）
- 正交相机 + 视图缩放（鼠标滚轮）

状态：**已可稳定用于当前沙盒场景**。

### 2) 物理系统（已实现核心，持续增强）

- `PhysicWorld` 封装 Chipmunk2D 世界
- 支持圆形 / 矩形 / 多边形刚体创建
- 支持绳索（`AddCord`）与弹簧（`AddSpring`）约束接口
- 支持全局重力调节
- 已实现静摩擦/动摩擦模型切换（基于接触点切向相对速度 + 阈值）

状态：**核心可用，教学展示足够；高级稳定性和边界工况仍可继续完善**。

### 3) 组件系统（已实现基础）

- `Component` 树形结构（父子关系）
- 基础变换：位置、缩放、旋转（含世界/相对接口）
- 组件与物理体同步（运行态物理驱动渲染，暂停态渲染驱动物理）
- Collider 可视化描边（暂停时显示）

状态：**可支撑当前对象编辑流程**。

### 4) 编辑与调试（已实现）

- ImGui 测试菜单（`tests/`）
- 支持：
  - 添加对象（按模板）
  - 选择对象
  - 编辑 Transform/颜色/物理参数
  - 运行/暂停/单步模拟
  - 编辑全局物理参数（重力、静摩擦、动摩擦、滑移阈值）
  - 保存/加载场景（`res/scenes/runtime_scene.json`）

状态：**已形成可用的“最小编辑器闭环”**。

### 5) 场景资源（已实现基础）

- Mesh 模板：`res/meshs/default.json`
- Component 模板：`res/Components/default.json`
- 运行时场景：`res/scenes/runtime_scene.json`

状态：**支持基础模板驱动与运行时序列化**。

## 项目结构 / Structure

- `src/Application.cpp`  
  主入口、ImGui 菜单/工具面板、场景保存加载。

- `src/MyWindow.*`  
  GLFW/GLEW 初始化、窗口循环、输入回调、视图缩放、选择逻辑。

- `src/renderComponents/`  
  渲染层封装（缓冲、Shader、Texture、Renderer）。

- `src/collisionSystem/PhysicWorld.*`  
  物理世界封装、刚体/形状创建、约束、全局摩擦模型。

- `src/Components/`  
  `Component` 树、渲染/物理同步、模板资源加载（`Sources`）。

- `src/tests/`  
  ImGui 测试与编辑页面（添加对象、属性编辑、设置、根节点浏览）。

- `src/vendor/`  
  第三方库（ImGui、stb_image、nlohmann/json 等）。

- `res/`  
  资源文件（shader、mesh、component 模板、场景）。

## 构建环境 / Build

- 语言标准：`C++14`
- 平台：Windows
- 常用工具链：Visual Studio
- 依赖：GLFW、GLEW、glm、Chipmunk2D、ImGui、stb_image、nlohmann/json

> 当前主要面向 Windows + VS 环境开发与调试。

## 近期开发建议路线 / Near-term Roadmap

1. 完善对象交互（拖拽、吸附、约束创建交互）
2. 增加可复现实验预设（如斜面、小车、单摆）
3. 强化场景格式（版本号、更多物理状态、容错加载）
4. 改善工程健壮性（内存与生命周期管理、异常路径）
5. 补充文档与演示截图/GIF

## 说明

本仓库以学习和毕业设计实现为目标，强调“可理解、可演示、可持续迭代”。
