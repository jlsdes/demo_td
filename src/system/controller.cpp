#include "controller.hpp"

#include "graphics/window.hpp"
#include "component/location.hpp"
#include "utils/config.hpp"
#include "utils/log.hpp"

#include <cassert>

#include "graphics/entity_component_system.hpp"


Controller::Controller( ECS * const ecs, Window & window, Camera & camera )
    : System { ecs }, m_input_manager { window.get_input_manager() }, m_camera { camera }, m_mouse_callback { 0 },
      m_key_callbacks {} {
    // Read the controls listed in the config, which are assumed to be there
    std::set const camera_keys {
        Config::get<int>( "Controls", "forward" ),
        Config::get<int>( "Controls", "backward" ),
        Config::get<int>( "Controls", "right" ),
        Config::get<int>( "Controls", "left" ),
        Config::get<int>( "Controls", "up" ),
        Config::get<int>( "Controls", "down" ),
        Config::get<int>( "Controls", "sprint" ),
    };

    auto const key_callback { [this]( int const key, int const action ) { this->camera_translation( key, action ); } };
    auto const callback_id { m_input_manager.observe_keyboard( camera_keys, key_callback ) };
    for ( int const key : camera_keys )
        m_key_callbacks.at( key ) = { callback_id, MoveCamera };

    auto const mouse_callback { [this]( double const x, double const y ) { camera_rotation( x, y ); } };
    m_mouse_callback = m_input_manager.observe_mouse( mouse_callback );

    // TODO change this to not capture the mouse, and rotate only on mouse button press
    glfwSetInputMode( glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED );
}

Controller::~Controller() {
    if ( m_mouse_callback )
        m_input_manager.forget_mouse( m_mouse_callback );
    System::~System();
}

void Controller::run() {}

void Controller::camera_translation( int const key, int const action ) const {
    Action const bound_action { m_key_callbacks.at( key ).action };
    assert( bound_action == MoveCamera );

    m_camera.toggle_movement( key, action );
}

void Controller::camera_rotation( double const x, double const y ) const {
    m_camera.rotate( { x, y } );
}
