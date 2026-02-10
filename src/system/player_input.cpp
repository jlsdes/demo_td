#include "player_input.hpp"

#include "component/location.hpp"
#include "graphics/entity_component_system.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"


unsigned int register_callbacks( InputManager & input_manager, PlayerInput & system ) {
    auto const callback { [&system]( int const key, int const action ) { system.player_input( key, action ); } };
    return input_manager.observe_keyboard( used_keys, callback );
}

PlayerInput::PlayerInput( ECS * const ecs, InputManager & input_manager )
    : System { ecs }, m_player_entity { 0 }, m_player_active { false }, m_controls { 0 }, m_input_counts { 0 },
      m_input_manager { input_manager }, m_callback_id { register_callbacks( input_manager, *this ) } {}

PlayerInput::~PlayerInput() {
    if ( m_callback_id )
        m_input_manager.forget_keyboard( used_keys, m_callback_id );
}

void PlayerInput::run() {
    auto const elapsed { static_cast<float>(Time::get_elapsed_time()) };

    for ( auto & location : m_ecs->components.get_array<Location>() )
        location.position += location.velocity * elapsed;
}

void PlayerInput::set_player_entity( EntityID const entity ) {
    m_player_entity = entity;
    m_player_active = true;
}

void PlayerInput::unset_player_entity() {
    m_player_active = false;
}

void PlayerInput::player_input( int const key, int const action ) {
    auto const camera_action { m_controls.at( key ) };
    if ( not camera_action ) {
        Log::warning( "PlayerInput system received a keyboard input for a key (", key, ") it was not expecting." );
        return;
    }
    if ( action == GLFW_PRESS )
        ++m_input_counts.at(camera_action);
    else if ( action == GLFW_RELEASE )
        --m_input_counts.at(camera_action);
    // 'action' could also be GLFW_REPEAT, which can be ignored
}
