#include "entity.hpp"

#include "component/component_manager.hpp"
#include "core/context.hpp"
#include "entity/entity_manager.hpp"
#include "utils/log.hpp"

#include <bit>


Entity::Entity( Context const & context ) : m_id { context.entities->create() }, m_context { context } {}

Entity::~Entity() {
    if ( m_moved )
        return;

    ComponentFlags components { m_context.entities->get_flags( m_id ) };
    while ( components ) {
        auto const component_type { std::countr_zero( components ) };
        m_context.components->remove_component( m_id, component_type );
        components ^= 1ull << component_type;
    }
    m_context.entities->remove( m_id );
}

Entity::Entity( Entity && other ) noexcept : m_id { other.m_id }, m_context { other.m_context } {
    if ( other.m_moved ) {
        Log::error( "Moved an already moved entity." );
        m_moved = true;
    }
    other.m_moved = true;
}
