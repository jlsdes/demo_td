#include "controller.hpp"

#include "component/drawable.hpp"
#include "component/location.hpp"
#include "entity/tile_highlight.hpp"
#include "core/entity_component_system.hpp"
#include "core/input_manager.hpp"
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


Controller::Controller( Context const & context, InputManager & input_manager, Camera & camera )
    : System { context }, m_input_manager { input_manager }, m_camera { camera }, m_callback_ids {}, m_flags { 0 } {
    for ( unsigned char action { 0 }; action < static_cast<unsigned char>(Camera::NumberActions); ++action ) {
        auto const key { Config::get<int>( "Controls", config_names[action] ) };
        auto const key_callback {
            [this, action]( int, int const key_action ) {
                handle_camera_key( static_cast<Camera::Action>(action), key_action );
            }
        };
        m_callback_ids[action] = m_input_manager.observe_input<KeyboardInput>( key_callback, key );
    }

    auto const cursor_callback { [this]( double const x, double const y ) { handle_cursor_movement( x, y ); } };
    m_callback_ids[CursorMove] = m_input_manager.observe_input<CursorInput>( cursor_callback );

    auto const mouse_callback { [this]( int, int const action ) { m_flags[IsCameraRotating] = action == GLFW_PRESS; } };
    m_callback_ids[CameraEnableRotate] = m_input_manager.observe_input<MouseButtonInput>(
        mouse_callback, GLFW_MOUSE_BUTTON_MIDDLE );

    auto const toggle_free_view {
        [this]( int, int const action ) { if ( action == GLFW_PRESS ) toggle_camera_mode(); }
    };
    m_callback_ids[CameraFreeView] = m_input_manager.observe_input<KeyboardInput>( toggle_free_view, GLFW_KEY_F );
}

Controller::~Controller() {
    for ( unsigned int const callback_id : m_callback_ids )
        m_input_manager.forget_input( callback_id );
    System::~System();
}

void Controller::run() {}

void Controller::handle_camera_key( Camera::Action const camera_action, int const key_action ) const {
    if ( key_action == GLFW_REPEAT )
        return;
    m_camera.toggle_movement( camera_action, key_action == GLFW_PRESS );
}

void Controller::handle_cursor_movement( double const x, double const y ) const {
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
