cmake_minimum_required(VERSION 4.0)
project(demo_td)

set(CMAKE_CXX_STANDARD 26)

# Using FetchContent to download some dependencies
include(FetchContent)

# Fetch GLFW
# GLFW may still require some dev packages, e.g. xorg-dev
FetchContent_Declare(
        glfw
        GIT_REPOSITORY  https://github.com/glfw/glfw.git
        GIT_TAG         3.4
)
FetchContent_MakeAvailable(glfw)

# Fetch GLAD
FetchContent_Declare(
        glad
        GIT_REPOSITORY  https://github.com/Dav1dde/glad.git
        GIT_TAG         v2.0.8
        GIT_PROGRESS    TRUE
        SOURCE_SUBDIR   cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_46 STATIC REPRODUCIBLE LOADER API gl:core=4.6)

# Fetch GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY  https://github.com/g-truc/glm.git
        GIT_TAG         1.0.2
)
FetchContent_MakeAvailable(glm)

# Build the main executable
add_executable(demo_td src/main.cpp
        src/engine/window.cpp
        src/engine/window.hpp
        src/engine/shader.cpp
        src/engine/shader.hpp
        src/engine/mesh.cpp
        src/engine/mesh.hpp
        src/engine/camera.cpp
        src/engine/camera.hpp
        src/engine/input_manager.cpp
        src/engine/input_manager.hpp
        src/engine/time.cpp
        src/engine/time.hpp
        src/engine/config.cpp
        src/engine/config.hpp
        src/engine/ini_reader.cpp
        src/engine/ini_reader.hpp
        src/engine/log.cpp
        src/engine/log.hpp
        src/engine/render_object.cpp
        src/engine/render_object.hpp
        src/engine/renderer.cpp
        src/engine/renderer.hpp
        src/engine/mesh_builder.cpp
        src/engine/mesh_builder.hpp
)

# Link the necessary libraries
target_link_libraries(demo_td PRIVATE glfw)
target_link_libraries(demo_td PRIVATE glad_gl_core_46)
target_link_libraries(demo_td PRIVATE glm)
