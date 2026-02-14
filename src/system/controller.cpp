#include "controller.hpp"

#include "graphics/entity_component_system.hpp"
#include "graphics/input_manager.hpp"
#include "component/location.hpp"
#include "utils/config.hpp"

#include <string>


std::string constexpr config_names[] {
    "forward", // Same order as the enum values of CameraAction
    "backward",
    "right",
    "left",
    "up",
    "down",
    "sprint",
};


Controller::Controller( ECS * const ecs, InputManager & input_manager, Camera & camera )
    : System { ecs }, m_input_manager { input_manager }, m_camera { camera }, m_callback_ids {}, m_flags { 0 } {
    for ( unsigned char action { 0 }; action < static_cast<unsigned char>(Camera::NumberActions); ++action ) {
        auto const key { Config::get<int>( "Controls", config_names[action] ) };
        auto const key_callback {
            make_callback<KeyboardInput>( [this, action]( int, int const key_action ) {
                if ( key_action != GLFW_REPEAT )
                    handle_camera_key( static_cast<Camera::Action>(action), key_action == GLFW_PRESS );
            } )
        };
        m_callback_ids[action] = m_input_manager.observe_input( KeyboardInput, key_callback, key );
    }

    CallbackFunction const cursor_callback {
        make_callback<CursorInput>( [this]( double const x, double const y ) { handle_camera_rotation( x, y ); } )
    };
    m_callback_ids[CameraRotate] = m_input_manager.observe_input( CursorInput, cursor_callback );

    CallbackFunction const mouse_callback {
        make_callback<MouseButtonInput>( [this]( int, int const action ) {
            m_flags[IsCameraRotating] = action == GLFW_PRESS;
        } )
    };
    m_callback_ids[CameraEnableRotate] = m_input_manager.observe_input( MouseButtonInput, mouse_callback,
                                                                        GLFW_MOUSE_BUTTON_MIDDLE );

    CallbackFunction const toggle_free_view {
        make_callback<KeyboardInput>( [this]( int, int const action ) {
            if ( action == GLFW_PRESS )
                toggle_camera_mode();
        } )
    };
    m_callback_ids[CameraFreeView] = m_input_manager.observe_input( KeyboardInput, toggle_free_view, GLFW_KEY_F );
}

Controller::~Controller() {
    for ( unsigned int const callback_id : m_callback_ids )
        m_input_manager.forget_input( callback_id );
    System::~System();
}

void Controller::run() {}

void Controller::handle_camera_key( Camera::Action const action, bool const is_pressing ) const {
    m_camera.toggle_movement( action, is_pressing );
}

void Controller::handle_camera_rotation( double const x, double const y ) const {
    static glm::vec2 previous_position { x, y };

    glm::vec2 const position { x, y };
    glm::vec2 const offset { position - previous_position };
    previous_position = position;

    if ( m_flags[IsCameraRotating] or m_flags[IsCameraFreeView] )
        m_camera.rotate( offset );
}

void Controller::toggle_camera_mode() {
    m_flags[IsCameraFreeView] = !m_flags[IsCameraFreeView];
    auto const cursor_mode { m_flags[IsCameraFreeView] ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL };
    glfwSetInputMode( glfwGetCurrentContext(), GLFW_CURSOR, cursor_mode );
}
