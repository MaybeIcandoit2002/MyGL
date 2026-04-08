# MyGL

基于 OpenGL + Chipmunk2D 的二维物理实验沙盒（本科毕业设计最终稿）。  
A 2D physics sandbox for mechanics experiments, built with OpenGL and Chipmunk2D.

---

## 项目目的 / Project Goal

本项目的目标保持不变：

- 以 C++14 构建可运行、可交互的 2D 渲染与物理实验平台；
- 通过可视化编辑与调试界面降低实验搭建与参数调节门槛；
- 面向教学演示与课程实验场景，提供可复现实验流程。

> 说明：本仓库当前版本为毕业设计最终稿，功能以“可用、可演示、可复现”为主，不定位为通用游戏引擎。

---

## 当前版本状态（最终稿）

项目主体功能已完成，当前代码可稳定支持：

- 对象创建、选择、拖拽与属性编辑；
- 物理模拟运行/暂停、全局参数调节；
- 场景保存、加载、删除；
- 关键操作确认弹窗（可在 Setting 中启用/关闭）；
- 描述组件（文本说明）随对象相对位置跟随显示。

---

## 预设实验场景（已完成）

当前版本内置 3 个预设场景（`res/scenes/`）：

1. `单摆实验.json`
2. `弹簧简谐运动.json`
3. `斜坡小车.json`

可在运行时通过场景面板直接加载。

---

## 核心功能清单（按现版本）

### 1) 渲染系统

- 渲染封装：`Renderer`、`VertexBuffer`、`IndexBuffer`、`VertexArray`、`UniformBuffer`、`ShaderProgram`
- 实例化绘制（`glDrawElementsInstanced`）
- 纹理槽管理（当前最多 8 槽）
- 正交视图、缩放与视窗平移

### 2) 物理系统

- `PhysicWorld` 封装 Chipmunk2D
- 圆形 / 矩形 / 多边形碰撞体
- 关节与约束：绳索（Cord）、刚性杆（Rod）、弹簧（Spring）
- 全局重力、静摩擦、动摩擦、滑移阈值调节

### 3) 组件与编辑系统

- `Component` 树形结构（父子关系）
- 位置 / 缩放 / 旋转编辑（世界与相对接口）
- 运行态物理驱动渲染、暂停态渲染回写物理
- 描述组件（多行文本、参数附加显示、字体与行距控制）

### 4) 编辑器交互（ImGui）

- 添加对象（模板驱动）
- 选择、属性编辑、删除
- 运行/暂停模拟
- 场景保存/加载/删除
- 确认弹窗（确认/取消，居中模态）

---

## 项目结构 / Structure

- `src/Application.cpp`  
  主入口、主 UI 流程、场景序列化（保存/加载/删除）。

- `src/MyWindow.*`  
  窗口初始化、主循环、输入回调、视图控制、选择/拖拽。

- `src/collisionSystem/PhysicWorld.*`  
  物理世界、约束管理、全局摩擦模型。

- `src/Components/`  
  组件树、渲染/物理同步、描述组件逻辑。

- `src/renderComponents/`  
  OpenGL 渲染基础封装。

- `src/tests/`  
  编辑器面板（AddNewObject / EditProperties / Setting / SearchRoot 等）。

- `res/`  
  Shader、模板资源、预设场景。

---

## 构建环境 / Build

- 语言标准：`C++14`
- 平台：Windows
- 推荐工具链：Visual Studio
- 依赖：GLFW、GLEW、glm、Chipmunk2D、ImGui、stb_image、nlohmann/json

---

## 唯一未完成计划 / Remaining Plan

当前仅剩一个后续测试项：**负载能力测试**。

计划方式：

- 在同一场景中放置 `10 / 100 / 1000 / ...` 个小球；
- 记录并对比帧率等运行指标；
- 评估渲染与物理更新在不同规模下的性能变化。

---

## 说明

本仓库以学习与毕业设计实现为核心，强调：

- 可理解（结构清晰）
- 可演示（实验可复现）
- 可维护（便于后续优化与扩展）
