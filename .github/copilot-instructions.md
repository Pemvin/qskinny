# QSkinny Copilot 使用说明

## 总览
QSkinny 是基于 Qt 场景图的 C++ UI 框架，提供轻量级、高性能的 UI 控件，支持 C++ 和 QML。架构强调关注点分离：核心逻辑、样式（设计系统）、渲染均为可扩展的独立模块。


## 主要架构与子项目模块说明
- **src/**：核心库，按功能分模块（如 `controls/`、`layouts/`、`graphic/`）。所有控件均继承自 QskControl 或相关基类。
- **designsystems/**：皮肤/主题实现，包含如 `material3/`、`fluent2/`、`fusion/` 等多种设计系统，支持运行时动态切换皮肤。
- **examples/**：示例程序集合，`gallery/` 为综合演示所有控件和特性，`myapp/` 为最小模板，其他如 `buttons/`、`tabview/` 等为单控件或特性演示。
- **playground/**：实验性和开发中控件、布局、特性等的试验场，便于快速原型和新特性验证，内容经常变动，适合探索和调试。
- **support/**：全局辅助模块，包括字体资源、全局命名空间、快捷键（如 `SkinnyShortcut.h`）、形状工厂等，提供跨模块的基础能力。
- **tools/**：开发和设计辅助工具，主要为矢量图（QVG）相关，如 `svg2qvg`、`glyph2qvg`，用于将 SVG 或字体图标转换为 QSkinny 专用格式。


## 构建与使用
- 标准构建流程：
  ```
  mkdir build && cd build
  cmake ..
  cmake --build .
  cmake --install . --prefix <install_path>
  ```
- 指定 Qt 版本：设置 `CMAKE_PREFIX_PATH` 或使用 `qt-cmake`。
- 示例程序：见 `examples/myapp/` 和 `examples/gallery/`。
- 皮肤/插件：如需自定义路径，运行时设置 `QT_PLUGIN_PATH`（详见 Qt 文档）。

## 项目约定
- **控件**：新控件放在 `src/controls/`，继承 QskControl，采用 skin/skinlet 样式分离模式。
- **设计系统**：新皮肤放在 `designsystems/`，并通过 QskSkinManager 注册。
- **快捷键**：全局快捷键统一用 `SkinnyShortcut::enable()`。
- **QML 最小化**：绝大多数逻辑用 C++ 实现，QML 导出可选，由构建参数控制。
- **CMake**：新示例用 `qsk_add_example()`，参考 `examples/myapp/CMakeLists.txt`。

## 开发者工作流
- **调试**：使用标准 Qt 调试工具。皮肤/插件问题重点检查 `QT_PLUGIN_PATH` 和皮肤注册。
- **测试**：无正式测试套件，回归主要依赖示例程序。
- **文档**：API 文档用 Doxygen 生成，见 `doc/generate-website.md`。

## 集成要点
- **Qt 依赖**：需 Qt >= 5.15（推荐 Qt 6.x），直接使用 QtQuick 场景图。
- **皮肤切换**：运行时通过 QskSkinManager 动态切换皮肤。
- **矢量图**：自定义 QVG 格式，相关工具为 `svg2qvg`/`glyph2qvg`。

## 典型示例
- 最小应用：`examples/myapp/main.cpp`
- 综合演示：`examples/gallery/`
- 新控件开发：参考 `src/controls/QskPushButton.h` 及相关 skinlet 文件。

---
更多细节见 [入门教程](../doc/tutorials/03-writing-your-first-application.asciidoc) 和 `README.md`。
