#include "entity.hpp"
#include "core/entity_component_system.hpp"
#include "utils/log.hpp"

#include <bit>


Entity::Entity( ECS * const ecs ) : m_id { ecs->entities.create() }, m_ecs { ecs } {}

Entity::~Entity() {
    if ( m_moved )
        return;

    ComponentFlags components { m_ecs->entities.get_flags( m_id ) };
    while ( components ) {
        auto const component_type { std::countr_zero( components ) };
        m_ecs->components.remove_component( m_id, component_type );
        components ^= 1ull << component_type;
    }
    m_ecs->entities.remove( m_id );
}

Entity::Entity( Entity && other ) noexcept : m_id { other.m_id }, m_ecs { other.m_ecs } {
    if ( other.m_moved ) {
        Log::error( "Moved an already moved entity." );
        m_moved = true;
    }
    other.m_moved = true;
}
