#include "window.hpp"
#include "GLFW/glfw3.h"

#include <stdexcept>


Window::Window()
        : m_window { glfwCreateWindow( 800, 600, "Demo TD", nullptr, nullptr ) } {
    if ( not m_window )
        throw std::runtime_error( "Failed to create a window." );
    focus();
    if ( not gladLoadGL( glfwGetProcAddress ) )
        throw std::runtime_error( "Failed to initialise GLAD." );
    glClearColor( 0.1, 0.1, 0.1, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glfwSetKeyCallback( m_window, []( GLFWwindow * const window, int const key, int, int const action, int ) {
        bool const correct_key { key == GLFW_KEY_CAPS_LOCK or key == GLFW_KEY_ESCAPE };
        bool const correct_action { action == GLFW_PRESS };
        if ( correct_key and correct_action )
            glfwSetWindowShouldClose( window, true );
    } );

}

Window::~Window() {
    glfwDestroyWindow(m_window);
}

void Window::focus() const {
    glfwMakeContextCurrent( m_window );
}

void Window::draw() const {
    glfwSwapBuffers( m_window );
}

bool Window::is_closing() const {
    return glfwWindowShouldClose( m_window );
}
