#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/window.hpp"


int main()
{
    Window window { 600, 480, "Demo TD" };

    while ( !window.is_closing() ) {
        window.render();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
