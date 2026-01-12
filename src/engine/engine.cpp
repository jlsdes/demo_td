#include "engine.hpp"
#include "utils/log.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>


template <typename T>
bool pop_manager( std::vector<std::unique_ptr<T>> & container, T const * manager, std::string const & name ) {
    for ( auto iterator { container.cbegin() }; iterator != container.cend(); ++iterator ) {
        if ( iterator->get() != manager )
            continue;
        container.erase( iterator );
        return true;
    }
    Log::error( "Attempted to remove a ", name, " that couldn't be found." );
    return false;
}

void initialise_glfw() {
    if ( not glfwInit() ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLFW" );
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 ); // Enable MSAA (anti-aliasing)
}

void initialise_glad() {
    if ( not gladLoadGL( glfwGetProcAddress ) ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLAD." );
    }
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
}

Engine::Engine() : m_window { nullptr }, m_models {}, m_views {}, m_controllers {} {
    static bool initialised = false;

    if ( not initialised )
        initialise_glfw();
    m_window = std::make_unique<Window>();
    if ( not initialised )
        initialise_glad();

    initialised = true;
}

void Engine::push_model_manager( std::unique_ptr<ModelManager> && model_manager ) {
    m_models.emplace_back( std::move( model_manager ) );
}

bool Engine::pop_model_manager( ModelManager const * model_manager ) {
    return pop_manager( m_models, model_manager, "ModelManager" );
}

void Engine::push_view_manager( std::unique_ptr<Renderer> && view_manager ) {
    m_views.emplace_back( std::move( view_manager ) );
}

bool Engine::pop_view_manager( Renderer const * view_manager ) {
    return pop_manager( m_views, view_manager, "Renderer" );
}

void Engine::push_controller_manager( std::unique_ptr<ControllerManager> && controller_manager ) {
    m_controllers.emplace_back( std::move( controller_manager ) );
}

bool Engine::pop_controller_manager( ControllerManager const * controller_manager ) {
    return pop_manager( m_controllers, controller_manager, "ControllerManager" );
}

void Engine::game_loop() {}

void Engine::render_loop() {}

Window & Engine::get_window() const {
    return *m_window;
}
