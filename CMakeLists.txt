cmake_minimum_required(VERSION 4.0)
project(demo_td)

set(CMAKE_CXX_STANDARD 26)

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

        src/engine/camera.cpp
        src/engine/camera.hpp
        src/engine/input_manager.cpp
        src/engine/input_manager.hpp
        src/engine/mesh.hpp
        src/engine/mesh_builder.cpp
        src/engine/mesh_builder.hpp
        src/engine/shader.cpp
        src/engine/shader.hpp
        src/engine/window.cpp
        src/engine/window.hpp

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
        src/engine/entity_component_system.cpp
        src/engine/entity_component_system.hpp
        src/component/position.hpp
        src/engine/mesh.ipp
        src/engine/context.cpp
        src/engine/context.hpp
        src/entity/tower_factory.cpp
        src/entity/tower_factory.hpp
        src/component/tower_data.hpp
)

# Link the necessary libraries
target_link_libraries(demo_td PRIVATE glfw)
target_link_libraries(demo_td PRIVATE glad_gl_core_46)
target_link_libraries(demo_td PRIVATE glm)
