# Lemur

A lightweight, immediate-draw GUI library built on [Blend2D](https://blend2d.com/) and [GLFW](https://www.glfw.org/).

## Features

- **Widget hierarchy**: `Control` → `Container`, `Label`, `Button`, `TextBox`
- **Flexible layout**: Absolute, Vertical, Horizontal, Grid modes with flex-grow and anchor support
- **Scrolling**: Auto/Always/Hidden vertical and horizontal scrollbars per Container
- **Input routing**: Mouse capture, focus management, keyboard/text dispatch
- **Style system**: Per-widget `ControlStyle` struct; stylesheet support (planned)
- **Platform agnostic**: No OS calls — host injects GLFW events via `dispatchXxx()` methods

## Quick start

```cpp
#include <Lemur/Lemur.h>

// Create a root container
auto root = std::make_shared<Lemur::Container>("root");
root->setBounds(0, 0, 1024, 768);
root->setLayoutMode(Lemur::LayoutMode::Vertical);

// Add a label
auto lbl = root->addChild<Lemur::Label>("Hello, Lemur!");
lbl->setFont(myFont);
lbl->setBounds(0, 0, 300, 40);

// Render each frame
BLContext ctx(image);
root->draw(ctx);
ctx.end();
```

## CMake integration

```cmake
add_subdirectory(libs/Lemur)
target_link_libraries(MyApp PRIVATE lemur)
```

Lemur needs Blend2D and GLFW headers available on the include path. See the parent project's `CMakeLists.txt` for an example.

## Namespace

All types live in the `Lemur` namespace: `Lemur::Control`, `Lemur::Button`, etc.
