#include "window.hpp"
#include "input_manager.hpp"
#include "window_context.hpp"

#include <stdexcept>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on


Window::Window() : m_window { glfwCreateWindow( 800, 600, "Demo TD", nullptr, nullptr ) } {
    if ( not m_window )
        throw std::runtime_error( "Failed to create a window." );
    focus();
    if ( not gladLoadGL( glfwGetProcAddress ) )
        throw std::runtime_error( "Failed to initialise GLAD." );
    glClearColor( 0.1, 0.1, 0.1, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
}

Window::~Window() {
    glfwDestroyWindow( m_window );
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

void Window::initialise() {
    KeyboardObserver callback { [this]( int, int, int const action, int ) {
        glfwSetWindowShouldClose( m_window, action == GLFW_PRESS );
    } };

    auto const context { reinterpret_cast<WindowContext *>( glfwGetWindowUserPointer( m_window ) ) };
    context->input_manager.add_observer( { callback, GLFW_KEY_ESCAPE } );
    context->input_manager.add_observer( { callback, GLFW_KEY_CAPS_LOCK } );
}
