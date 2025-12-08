#include "window.hpp"

#include <stdexcept>


Window::Window( unsigned int const width, unsigned int const height, char const * const title )
    : m_window { nullptr }
{
    // Initialise GLFW
    if ( !glfwInit() ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLFW" );
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    // Enable MSAA (anti-aliasing)
    glfwWindowHint( GLFW_SAMPLES, 8 );

    // Create the new window using GLFW
    m_window = glfwCreateWindow( static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr );
    if ( m_window == nullptr ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to create window." );
    }

    // Set the newly created window as active
    focus();

    // Initialise GLAD
    if ( !gladLoadGL( glfwGetProcAddress ))
        throw std::runtime_error( "Failed to initialise GLAD." );

    // Set a background colour if nothing gets drawn
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

    // The window user pointer will allow us to retrieve this object from the GLFW window object; useful for callbacks
    glfwSetWindowUserPointer( m_window, this );
    glfwSetFramebufferSizeCallback( m_window, resize_callback );
    glfwSetKeyCallback( m_window, keyboard_callback );  // TODO move this to a separate class probably
}

Window::~Window()
{
    if ( m_window != nullptr ) {
        glfwDestroyWindow( m_window );
        m_window = nullptr;
    }
}

void Window::focus() const
{
    glfwMakeContextCurrent( m_window );
}

void Window::render() const
{
    glfwSwapBuffers( m_window );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Window::resize( unsigned int const width, unsigned int const height ) const
{
    focus();
    glViewport( 0, 0, static_cast<int>( width ), static_cast<int>( height ) );
}

void Window::close() const
{
    glfwSetWindowShouldClose( m_window, true );
}

bool Window::is_closing() const
{
    return glfwWindowShouldClose( m_window );
}

void Window::resize_callback( GLFWwindow * const glfw_window, int const width, int const height )
{
    auto const window = static_cast<Window *>( glfwGetWindowUserPointer( glfw_window ) );
    window->resize( width, height );
}

void Window::keyboard_callback( GLFWwindow * const glfw_window, int const key, int const scancode, int const action, int const mods )
{
    auto const window = static_cast<Window *>( glfwGetWindowUserPointer( glfw_window ) );
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        window->close();
}
