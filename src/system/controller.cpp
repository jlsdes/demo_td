#include "controller.hpp"

#include "graphics/window.hpp"
#include "utils/log.hpp"


Controller::Controller( ECS * const ecs, Window & window )
    : System { ecs }, m_input_manager { window.get_input_manager() } {
    m_mouse_callback =
        m_input_manager.observe_mouse( []( double const x, double const y ) {
            Log::debug( "Mouse is at ", x, ", ", y );
        } );
}

Controller::~Controller() {
    m_input_manager.forget_mouse( m_mouse_callback );
    System::~System();
}

void Controller::run() {}
