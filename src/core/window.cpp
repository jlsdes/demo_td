#include "window.hpp"
#include "input_manager.hpp"
#include "window_context.hpp"

#include <stdexcept>

#include <lib/gl.hpp>


unsigned int constexpr initial_width { 800 };
unsigned int constexpr initial_height { 600 };


Window::Window()
        : m_window { glfwCreateWindow( initial_width, initial_height, "Demo TD", nullptr, nullptr ) },
          m_width { initial_width },
          m_height { initial_height } {
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

std::pair<unsigned int, unsigned int> Window::get_size() const {
    return { m_width, m_height };
}

void Window::set_size( std::pair<unsigned int, unsigned int> const & size ) {
    glfwSetWindowSize( m_window, size.first, size.second );
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

void Window::initialise( WindowContext & context ) {
    KeyboardObserver close_callback { [this]( int, int, int const action, int ) {
        glfwSetWindowShouldClose( m_window, action == GLFW_PRESS );
    } };
    context.input_manager.add_observer( { close_callback, GLFW_KEY_ESCAPE } );
    context.input_manager.add_observer( { close_callback, GLFW_KEY_CAPS_LOCK } );

    ResizeObserver resize_callback { [this]( unsigned int const width, unsigned int const height ) {
        m_width  = width;
        m_height = height;
        glViewport( 0, 0, m_width, m_height );
    } };
    context.input_manager.add_observer( { resize_callback } );
}
