#include "movement.hpp"

#include "component/component_manager.hpp"
#include "component/location.hpp"
#include "entity/entity_manager.hpp"
#include "utils/time.hpp"


unsigned int register_callbacks( InputManager & input_manager, Movement & system ) {
    auto const callback { [&system]( int const key, int const action ) { system.player_input( key, action ); } };
    return input_manager.observe_keyboard( used_keys, callback);
}

Movement::Movement( InputManager & input_manager ) : m_player_entity { 0 }, m_player_active { false },
                                                     m_input_manager { input_manager },
                                                     m_callback_id { register_callbacks( input_manager, *this ) } {}

Movement::Movement( InputManager & input_manager, EntityID const player_entity )
    : m_player_entity { player_entity }, m_player_active { true }, m_input_manager { input_manager },
      m_callback_id { register_callbacks( input_manager, *this ) } {}

Movement::~Movement() {
    if ( m_callback_id )
        m_input_manager.forget_keyboard( used_keys, m_callback_id );
}

void Movement::run( EntityManager const & entities, ComponentManager & components ) {
    auto const elapsed { static_cast<float>(Time::get_elapsed_time()) };

    for ( auto & location : components.get_array<Location>() )
        location.position += location.velocity * elapsed;
}

void Movement::set_player_entity( EntityID const entity ) {
    m_player_entity = entity;
    m_player_active = true;
}

void Movement::unset_player_entity() {
    m_player_active = false;
}

void Movement::player_input( int const key, int const action ) {}
