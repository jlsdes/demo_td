#include "window.hpp"

#include <stdexcept>


Window::Window( unsigned int const width, unsigned int const height, char const * const title )
    : m_window { nullptr }, m_input_manager {} {
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
    if ( !gladLoadGL( glfwGetProcAddress ) )
        throw std::runtime_error( "Failed to initialise GLAD." );

    // General setup for OpenGL
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CW );

    // The window user pointer will allow us to retrieve this object from the GLFW window object; useful for callbacks
    glfwSetWindowUserPointer( m_window, this );
    glfwSetFramebufferSizeCallback( m_window, resize_callback );

    // The input manager can only be set up once GLFW has been initialised
    InputManager::initialise( m_window );
    auto close_callback = [this]( int, int ) { this->close(); };
    m_input_manager.observe_keyboard( { GLFW_KEY_ESCAPE, GLFW_KEY_CAPS_LOCK }, close_callback );
}

Window::~Window() {
    if ( m_window != nullptr ) {
        glfwDestroyWindow( m_window );
        m_window = nullptr;
    }
}

void Window::focus() const {
    glfwMakeContextCurrent( m_window );
}

void Window::render() const {
    glfwSwapBuffers( m_window );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
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
