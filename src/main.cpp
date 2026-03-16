// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdexcept>

#include "core/shader.hpp"
#include "core/window.hpp"


void initialise_glfw() {
    if ( not glfwInit() )
        throw std::runtime_error( "Failed to initialise GLFW." );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 );
}


int main() {
    initialise_glfw();
    Window window {};

    Shader shader { "./src/shader/main.vert", "./src/shader/main.frag" };

    while ( not window.is_closing() ) {
        glfwPollEvents();
        window.draw();
    }

    return 0;
}
