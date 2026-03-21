#pragma once

#include "camera.hpp"
#include "input_manager.hpp"
#include "window.hpp"


struct WindowContext {
    Window window;
    InputManager input_manager;
    Camera camera;

    WindowContext() : window {}, input_manager {}, camera {} {
        glfwSetWindowUserPointer( glfwGetCurrentContext(), this );

        window.initialise( *this );
        camera.initialise( *this );
    }
};
