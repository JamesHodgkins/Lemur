cmake_minimum_required(VERSION 3.20)
project(Lemur LANGUAGES CXX)

# ── Sources ───────────────────────────────────────────────────────────────────
add_library(lemur STATIC
    src/Control.cpp
    src/Container.cpp
    src/Label.cpp
    src/Button.cpp
    src/TextBox.cpp
    src/Stylesheet.cpp
)

# ── Public include path ───────────────────────────────────────────────────────
# Consumers do: #include <Lemur/Control.h> etc.
target_include_directories(lemur PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ── Language standard ─────────────────────────────────────────────────────────
target_compile_features(lemur PUBLIC cxx_std_20)

# ── Blend2D (required) ────────────────────────────────────────────────────────
# The parent project is responsible for making blend2d headers available.
# We inherit the include path via the PARENT_SCOPE include_directories set
# in the root CMakeLists.txt.  If consuming standalone, set:
#   target_include_directories(lemur PUBLIC /path/to/blend2d)
# GLFW is needed only by TextBox (for GLFW_KEY_* constants).

# ── MSVC warnings ─────────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(lemur PRIVATE /W3 /sdl /MP)
endif()
