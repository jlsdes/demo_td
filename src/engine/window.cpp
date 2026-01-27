#include "window.hpp"
#include "utils/config.hpp"

#include <stdexcept>


Window::Window()
    : Window(
        Config::get<unsigned int>( "Window", "width" ),
        Config::get<unsigned int>( "Window", "height" ),
        Config::get<char const *>( "Window", "title" )
    ) {}

Window::Window( unsigned int const width, unsigned int const height, char const * const title )
    : m_window { glfwCreateWindow( static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr ) } {
    // Create the new window using GLFW
    if ( not m_window ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to create window." );
    }
    focus();

    // The window user pointer will allow us to retrieve this object from the GLFW window object; useful for callbacks
    glfwSetWindowUserPointer( m_window, this );
    glfwSetFramebufferSizeCallback( m_window, resize_callback );

    // The input manager can only be set up once GLFW has been initialised
    InputManager::initialise( m_window );
    auto close_callback = [this]( int, int ) { this->close(); };
    m_input_manager.observe_keyboard( { GLFW_KEY_ESCAPE, GLFW_KEY_CAPS_LOCK }, close_callback );
}

Window::~Window() {
    if ( m_window )
        glfwDestroyWindow( m_window );
}

void Window::focus() const {
    glfwMakeContextCurrent( m_window );
}

void Window::clear() const {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Window::render() const {
    glfwSwapBuffers( m_window );
}

void Window::resize( unsigned int const width, unsigned int const height ) const {
    focus();
    glViewport( 0, 0, static_cast<int>(width), static_cast<int>(height) );
}

void Window::close() const {
    glfwSetWindowShouldClose( m_window, true );
}

bool Window::is_closing() const {
    return glfwWindowShouldClose( m_window );
}

InputManager & Window::get_input_manager() {
    return m_input_manager;
}

void Window::resize_callback( GLFWwindow * const glfw_window, int const width, int const height ) {
    auto const window = static_cast<Window *>(glfwGetWindowUserPointer( glfw_window ));
    window->resize( width, height );
}
