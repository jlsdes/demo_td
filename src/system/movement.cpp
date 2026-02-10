#include "movement.hpp"

#include "component/location.hpp"
#include "graphics/entity_component_system.hpp"
#include "utils/time.hpp"


unsigned int register_callbacks( InputManager & input_manager, Movement & system ) {
    auto const callback { [&system]( int const key, int const action ) { system.player_input( key, action ); } };
    return input_manager.observe_keyboard( used_keys, callback );
}

Movement::Movement( ECS * const ecs, InputManager & input_manager )
    : System { ecs }, m_player_entity { 0 }, m_player_active { false }, m_controls { 0 }, m_input_counts { 0 },
      m_input_manager { input_manager }, m_callback_id { register_callbacks( input_manager, *this ) } {}

Movement::Movement( ECS * const ecs, InputManager & input_manager, EntityID const player_entity )
    : System { ecs }, m_player_entity { player_entity }, m_player_active { true }, m_controls { 0 },
      m_input_counts { 0 }, m_input_manager { input_manager },
      m_callback_id { register_callbacks( input_manager, *this ) } {
    for ( auto const & [key, action] : default_controls )
        m_controls.at( key ) = action;
}

Movement::~Movement() {
    if ( m_callback_id )
        m_input_manager.forget_keyboard( used_keys, m_callback_id );
}

void Movement::run() {
    auto const elapsed { static_cast<float>(Time::get_elapsed_time()) };

    for ( auto & location : m_ecs->components.get_array<Location>() )
        location.position += location.velocity * elapsed;
}

void Movement::set_player_entity( EntityID const entity ) {
    m_player_entity = entity;
    m_player_active = true;
}

void Movement::unset_player_entity() {
    m_player_active = false;
}

void Movement::player_input( int const key, int const action ) {
    // Another valid action is GLFW_REPEAT, which should be ignored, but other random value could (but shouldn't) occur
    if ( action != GLFW_PRESS and action != GLFW_RELEASE )
        return;

    auto const camera_action { m_controls.at( key ) };
    if ( not camera_action ) {
        Log::warning( "Movement system received a keyboard input for a key (", key, ") it was not expecting." );
        return;
    }
    m_input_counts.at( camera_action ) += action == GLFW_PRESS ? 1 : -1;
}
