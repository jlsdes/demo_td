// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <memory>
#include <stdexcept>
#include <vector>

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

    unsigned int constexpr nr_windows { 2 };
    std::vector<std::unique_ptr<Window>> windows { nr_windows };
    for ( unsigned int i { 0 }; i < windows.size(); ++i )
        windows[i] = std::make_unique<Window>();

    while ( not windows.empty() ) {
        unsigned int index { 0 };
        while ( index < windows.size() ) {
            windows[index]->focus();
            glfwPollEvents();
            windows[index]->draw();

            if ( windows[index]->is_closing() )
                windows.erase( windows.begin() + index );
            else
                ++index;
        }
    }

    return 0;
}
