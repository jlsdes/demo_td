#include "engine.hpp"
#include "utils/log.hpp"

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

Engine::Engine() {
    initialise_glfw();
    m_window = Window();
    initialise_glad();
}

void Engine::initialise_glfw() {}

void Engine::initialise_glad() {}

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
