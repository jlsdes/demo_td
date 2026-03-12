cmake_minimum_required(VERSION 4.0)
project(demo_td)

set(CMAKE_CXX_STANDARD 26)

# Required for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Also required for clangd, as otherwise commands that are not supported by clangd would be generated
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Using FetchContent to download some dependencies
include(FetchContent)

# Fetch GLFW
# GLFW may still require some dev packages, e.g. xorg-dev
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# Fetch GLAD
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.8
        GIT_PROGRESS TRUE
        SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_46 STATIC REPRODUCIBLE LOADER API gl:core=4.6)

# Fetch GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

include_directories(src)

# Build the main executable
add_executable(demo_td
        src/main.cpp

        src/core/camera.cpp
        src/core/camera.hpp
        src/core/input_manager.cpp
        src/core/input_manager.hpp
        src/core/mesh.hpp
        src/core/mesh_builder.cpp
        src/core/mesh_builder.hpp
        src/core/shader.cpp
        src/core/shader.hpp
        src/core/window.cpp
        src/core/window.hpp

        src/utils/config.cpp
        src/utils/config.hpp
        src/utils/ini_reader.cpp
        src/utils/ini_reader.hpp
        src/utils/log.cpp
        src/utils/log.hpp
        src/utils/time.hpp
        src/utils/time.cpp

        src/entity/entity_manager.cpp
        src/entity/entity_manager.hpp
        src/component/component_manager.cpp
        src/component/component_manager.hpp
        src/system/system.hpp
        src/component/component.hpp
        src/entity/entity.hpp
        src/system/system_manager.cpp
        src/system/system_manager.hpp
        src/system/system_manager.ipp
        src/component/drawable.hpp
        src/system/renderer.cpp
        src/system/renderer.hpp
        src/component/location.hpp
        src/core/mesh.ipp
        src/core/context.cpp
        src/core/context.hpp
        src/entity/tower.cpp
        src/entity/tower.hpp
        src/component/tower_data.hpp
        src/component/terrain_tile.hpp
        src/utils/image.cpp
        src/utils/image.hpp
        src/system/movement.cpp
        src/system/movement.hpp
        src/system/controller.cpp
        src/system/controller.hpp
        src/entity/tile_highlight.cpp
        src/entity/tile_highlight.hpp
        src/entity/entity.cpp
        src/component/entity_type.hpp
        src/system/tile_manager.cpp
        src/system/tile_manager.hpp
)

# Link the necessary libraries
target_link_libraries(demo_td PRIVATE glfw)
target_link_libraries(demo_td PRIVATE glad_gl_core_46)
target_link_libraries(demo_td PRIVATE glm)
