# MyGL

基于 Chipmunk2D 的中学物理力学沙盒实验室 / A Chipmunk2D-based 2D Physics Sandbox Lab

A personal undergraduate capstone project: a 2D physics sandbox lab based on Chipmunk2D.
个人本科毕业设计项目：基于 Chipmunk2D 的二维物理力学沙盒实验室，用于中学物理力学实验的可视化与交互。

The goal of this project ("基于 Chipmunk2D 的中学物理力学沙盒实验室的设计与实现") is to build a small learning/experimental 2D engine demo that can be used to simulate and visualize basic mechanics experiments for middle-school physics.
本项目的目标（“基于 Chipmunk2D 的中学物理力学沙盒实验室的设计与实现”）是实现一个用于中学物理教学与自学的 2D 力学沙盒引擎 Demo，用来仿真和展示基础力学实验。

## Features (Current & Planned)
## 功能特性（当前 & 规划）

- 2D rendering core  
  二维渲染核心
  - OpenGL-based 2D renderer with batched drawing  
    基于 OpenGL 的 2D 渲染器，支持批量绘制
  - Custom `Renderer`, `VertexBuffer`, `IndexBuffer`, `VertexArray`, `UniformBuffer`, `ShaderProgram` wrappers  
    自定义渲染封装：`Renderer`、`VertexBuffer`、`IndexBuffer`、`VertexArray`、`UniformBuffer`、`ShaderProgram` 等

- Physics engine integration  
  物理引擎集成
  - Chipmunk2D-based world (`PhysicWorld`) with gravity, rigid bodies and basic shapes (circles, boxes, polygons)  
    基于 Chipmunk2D 的物理世界（`PhysicWorld`），支持重力、刚体以及基础形状（圆形、矩形、多边形）
  - Supports adding constraints such as cords and springs  
    支持添加绳索、弹簧等约束
  - Planned: distinguish static and kinetic friction in the simulation using configurable global coefficients (with optional per-object multipliers)  
    规划中：在仿真中通过可配置的全局静摩擦/动摩擦系数（可选每物体系数修正）区分静摩擦力和动摩擦力

- Component & UI system  
  组件与 UI 系统
  - `Component` tree with parent/child hierarchy and `UITransform` (position, scale, rotation)  
    基于 `Component` 的节点树结构，支持父子层级与 `UITransform`（位置、缩放、旋转）
  - Mesh & texture management via `Sources` and `Mesh` utilities  
    通过 `Sources` 和 `Mesh` 工具进行网格与纹理资源管理
  - Planned: basic UI layout system where containers auto-arrange children, while icons and scene objects use manual positioning  
    规划中：基础 UI 布局系统，其中容器自动排列子元素，而图标和场景元素采用手动定位

- Interaction & input  
  交互与输入
  - Planned: mouse-based interaction including click, drag and hover on components, using simple top-level hit detection (no complex overlapping/bubbling)  
    规划中：基于鼠标的组件交互，支持点击、拖拽、悬浮事件，采用简单的顶层命中检测（不处理复杂重叠/事件冒泡）
  - Planned: hit-testing powered by the physics engine (Chipmunk2D point queries) to detect which component is under the cursor  
    规划中：通过物理引擎（Chipmunk2D 的点查询）检测光标命中的组件
  - Planned: keyboard input used mainly for editing numeric parameters (e.g. mass, friction, initial velocity)  
    规划中：键盘输入主要用于编辑数值参数（如质量、摩擦系数、初速度等）

- Tooling & debug  
  调试与工具支持
  - ImGui-based test framework (`tests/Test*`) and debug UI  
    基于 ImGui 的测试框架（`tests/Test*`）和调试界面
  - Planned: ImGui panels to inspect and edit object parameters (physics and transform) at runtime  
    规划中：使用 ImGui 面板在运行时查看和修改物体参数（包括物理参数和变换）

- Experiment scenes  
  实验场景支持
  - Planned: 1–2 preset mechanics experiment scenes that can be loaded from the UI, such as an inclined-plane cart and a simple pendulum  
    规划中：预设 1–2 个典型力学实验场景，可通过 UI 载入，例如斜面小车、单摆等
  - Planned: scene save/load support so users can store and reload experiment setups, including component hierarchy, physics parameters and initial states  
    规划中：支持场景的保存与读取，包含组件层级、物理参数以及初始位置/速度等状态

## Project Status
## 项目状态

### Visual progress
### 进度可视化

- Rendering core / 渲染核心：`[########..]` ~80%
- Physics core / 物理核心：`[#######...]` ~70–80%
- Components & resources / 组件与资源管理：`[####......]` ~40–50%
- UI layout & interaction / UI 布局与交互：`[##........]` ~20% (设计中，仅有基础结构)
- Scene system & presets / 场景系统与预设实验：`[#.........]` ~10% (仅有资源加载与初步想法)
- Docs & polishing / 文档与优化：`[#.........]` ~10% (README 初稿已完成，其余待完善)

### Summary
### 概要

- Core rendering + physics framework: ~50–60% complete  
  渲染与物理核心框架：约完成 50–60%，基础功能已可用于构建简单实验场景
- Component system and resource management: ~40–50% complete  
  组件系统与资源管理：约完成 40–50%，已有基础节点树和 mesh/纹理管理
- UI layout, interaction, scene system: early design stage, minimal implementation  
  UI 布局、交互、场景系统：处于早期设计阶段，仅有少量实现
- Optimization, polishing, presets and documentation: not started (except this README)  
  优化、预设场景与文档完善：除本 README 外尚未开始

This repository is primarily for learning and experimentation and is not intended for production use.
本仓库主要用于学习与实验，不推荐直接用于生产环境。

## Build & Run
## 构建与运行

- Target platform: Windows  
  目标平台：Windows
- Toolchain: Visual Studio (C++14), builds to a standalone `.exe`  
  开发工具链：Visual Studio（C++14），编译为独立 `.exe` 可执行文件
- Dependencies (linked or included in `src/vendor`):  
  依赖库（通过工程链接或包含在 `src/vendor` 中）
  - GLFW (version: 3.4)
  - GLEW (version: 2.1.0)
  - glm (version: 1.60)
  - Chipmunk2D (version: 7.0.3)
  - ImGui, stb_image, nlohmann/json (bundled in `src/vendor`)

> Note: At this stage, the project only targets Windows + Visual Studio; cross‑platform builds are not a current goal.  
> 说明：当前仅面向 Windows + Visual Studio 环境构建，暂不考虑跨平台支持。

## Code Structure Overview
## 代码结构概览

- `src/Application.cpp` – program entry, window creation, main loop, ImGui test menu  
  程序入口，创建窗口与主循环，并集成 ImGui 测试菜单
- `src/MyWindow.*` – GLFW/GLEW initialization, OpenGL context, root `Component`, access to `Renderer` and `PhysicWorld`  
  封装 GLFW/GLEW 初始化、OpenGL 上下文、根 `Component`，并提供对 `Renderer` 和 `PhysicWorld` 的访问
- `src/renderComponents/` – rendering layer (buffers, shaders, renderer, textures, vertex formats)  
  渲染层：缓冲区、着色器、渲染器、纹理以及顶点格式等
- `src/collisionSystem/` – Chipmunk2D world wrapper (`PhysicWorld`), gravity, shapes, constraints  
  物理碰撞系统：`PhysicWorld` 封装 Chipmunk2D 世界，处理重力、形状和约束
- `src/Components/` – UI/scene components, mesh & image resource management, layout and input (in progress)  
  组件系统：UI/场景组件、网格与图片资源管理、布局与输入（开发中）
- `src/tests/` – ImGui-based test menu and sample scenes  
  基于 ImGui 的测试菜单和示例场景
- `src/vendor/` – third-party libraries (ImGui, stb_image, json.hpp, etc.)  
  第三方库（ImGui、stb_image、json.hpp 等）

## Roadmap
## 开发路线图

短期目标（Phase 1–2）  
Short-term goals (Phase 1–2)

1. Implement basic UI layout system for containers (panels/toolbars) that auto-arrange child elements, while keeping icons and scene objects manually positioned.  
   实现用于容器（面板、工具栏）的基础 UI 布局系统，实现子元素的自动排列，同时保留图标与场景物体的手动定位方式。
2. Add mouse click/drag/hover events and simple interaction for components; wire GLFW input into a central dispatcher, using Chipmunk2D point queries for hit-testing.  
   为组件添加鼠标点击、拖拽、悬浮等事件，并将 GLFW 输入接入统一的事件分发器，使用 Chipmunk2D 的点查询进行命中检测。
3. Integrate keyboard input as a way to edit numeric parameters in the UI/ImGui panels.  
   将键盘输入集成到 UI/ImGui 面板中，用于编辑各类数值参数。

中期目标（Phase 3–4）  
Mid-term goals (Phase 3–4)

4. Extend physics interactions to clearly distinguish static vs. kinetic friction using global configurable coefficients (and optional per-object multipliers); visualize their effects in experiments.  
   通过全局可配置的静摩擦/动摩擦系数（可选每物体系数修正）扩展物理交互，在实验中清晰展示两者差异。
5. Expose object and physics parameters via ImGui panels for interactive editing (mass, friction, initial velocity, constraints, etc.).  
   通过 ImGui 面板暴露物体与物理参数（质量、摩擦系数、初速度、约束等），支持运行时交互编辑。
6. Implement JSON-based scene save/load including component hierarchy, physics parameters and initial positions/velocities.  
   实现基于 JSON 的场景保存/加载，完整保存组件层级、物理参数以及初始位置/速度。

后期目标（Phase 5+）  
Long-term goals (Phase 5+)

7. Provide at least two preset teaching-oriented experiment scenes: an inclined-plane cart and a simple pendulum.  
   提供至少两个面向教学的预设实验场景：斜面小车和单摆。
8. Improve documentation, examples, and overall code quality/organization; align README and code comments with thesis writing needs.  
   完善文档与示例，优化代码质量和工程结构，并使 README 与代码注释更好地支撑毕业论文撰写。
