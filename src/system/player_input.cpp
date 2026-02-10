#include "player_input.hpp"

#include "component/location.hpp"
#include "graphics/entity_component_system.hpp"
#include "utils/log.hpp"


unsigned int register_callbacks( InputManager & input_manager, PlayerInput & system ) {
    auto const callback { [&system]( int const key, int const action ) { system.player_input( key, action ); } };
    return input_manager.observe_keyboard( used_keys, callback );
}

PlayerInput::PlayerInput( ECS * const ecs, InputManager & input_manager )
    : System { ecs }, m_camera_entity { 0 }, m_camera_active { false }, m_controls { 0 }, m_camera_inputs { 0 },
      m_input_manager { input_manager }, m_callback_id { register_callbacks( input_manager, *this ) } {}

PlayerInput::~PlayerInput() {
    if ( m_callback_id )
        m_input_manager.forget_keyboard( used_keys, m_callback_id );
}

void PlayerInput::run() {}

void PlayerInput::set_player_entity( EntityID const entity ) {
    m_camera_entity = entity;
    m_camera_active = true;
}

void PlayerInput::unset_player_entity() {
    m_camera_active = false;
}

void PlayerInput::player_input( int const key, int const action ) {
    auto const camera_action { m_controls.at( key ) };
    if ( not camera_action ) {
        Log::warning( "PlayerInput system received a keyboard input for a key (", key, ") it was not expecting." );
        return;
    }
    if ( action == GLFW_PRESS )
        ++m_camera_inputs.at( camera_action );
    else if ( action == GLFW_RELEASE )
        --m_camera_inputs.at( camera_action );
        // 'action' could also be GLFW_REPEAT, which can be ignored
    else
        return;

    Location & camera_location { m_ecs->components.get_component<Location>( m_camera_entity ) };
    camera_location.velocity = {
        (m_camera_inputs.at( Forward ) > 0) - (m_camera_inputs.at( Backward ) > 0),
        (m_camera_inputs.at( Right ) > 0) - (m_camera_inputs.at( Left ) > 0),
        (m_camera_inputs.at( Up ) > 0) - (m_camera_inputs.at( Down ) > 0),
    };
    if ( m_camera_inputs.at( Sprint ))
        camera_location.velocity *= 2.f;
}
