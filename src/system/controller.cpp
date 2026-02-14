#include "controller.hpp"

#include "graphics/input_manager.hpp"
#include "component/location.hpp"
#include "utils/config.hpp"
#include "utils/log.hpp"

#include <cassert>
#include <string>

#include "graphics/entity_component_system.hpp"


std::string constexpr config_names[] {
    "forward",
    "backward",
    "right",
    "left",
    "up",
    "down",
    "sprint",
};


Controller::Controller( ECS * const ecs, InputManager & input_manager, Camera & camera )
    : System { ecs }, m_input_manager { input_manager }, m_camera { camera }, m_callback_ids {} {
    //  Set up the camera controls first
    auto const key_callback {
        make_callback<KeyboardInput>( [this]( int const key, int const action ) { camera_translation( key, action ); } )
    };
    for ( unsigned char action { CameraKeyFirst }; action <= CameraKeyLast; ++action ) {
        auto const key { Config::get<int>( "Controls", config_names[action] ) };
        m_callback_ids[action] = m_input_manager.observe_input( KeyboardInput, key_callback, key );
    }

    CallbackFunction const mouse_callback {
        make_callback<CursorInput>( [this]( double const x, double const y ) { camera_rotation( x, y ); } )
    };
    m_callback_ids[CameraRotate] = m_input_manager.observe_input( CursorInput, mouse_callback );
}

Controller::~Controller() {
    for ( unsigned int const callback_id : m_callback_ids )
        m_input_manager.forget_input( callback_id );
    System::~System();
}

void Controller::run() {}

void Controller::camera_translation( int const key, int const action ) const {
    m_camera.toggle_movement( key, action );
}

void Controller::camera_rotation( double const x, double const y ) const {
    m_camera.rotate( { x, y } );
}
