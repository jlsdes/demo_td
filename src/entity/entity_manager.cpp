#include "entity_manager.hpp"

#include "graphics/entity_component_system.hpp"
#include "utils/log.hpp"

#include <bit>
#include <cassert>


EntityManager::EntityManager( ECS * ecs ) : m_components { ecs->components }, m_systems { ecs->systems },
                                            m_component_flags { 0 }, m_existing { 0 }, m_nr_entities { 0 },
                                            m_next_entity { 0 } {}

EntityID EntityManager::create() {
    assert( m_nr_entities < g_max_entities );

    unsigned int segment_index { m_next_entity / s_segment_size };
    unsigned int bit_index { m_next_entity % s_segment_size };

    // Check whether an entity with the m_next_entity ID already exists, and if so find a different ID that is available
    if ( m_existing[segment_index] & (1ull << bit_index) ) {
        // Full segments can quickly be skipped
        while ( m_existing[segment_index] == ~0ull )
            segment_index = (segment_index + 1) % s_array_size;
        // Get the first open spot within this segment
        bit_index = std::countr_one( m_existing[segment_index] );
        m_next_entity = segment_index * s_segment_size + bit_index;
    }
    m_existing[segment_index] ^= 1ull << bit_index;
    ++m_nr_entities;

    EntityID const entity { m_next_entity };
    m_next_entity = (m_next_entity + 1) % g_max_entities;
    return entity;
}

void EntityManager::remove( EntityID const entity ) {
    if ( not entity_exists( entity ) ) {
        Log::warning( "Attempted to remove an entity ", entity, " that does not exist, removing nothing." );
        return;
    }
    unsigned int const segment_index { entity / s_segment_size };
    unsigned int const bit_index { entity % s_segment_size };
    m_existing.at( segment_index ) ^= 1ull << bit_index;
    --m_nr_entities;
}

bool EntityManager::entity_exists( EntityID const entity ) const {
    unsigned int const segment_index { entity / s_segment_size };
    unsigned int const bit_index { entity % s_segment_size };
    return m_existing[segment_index] & (1ull << bit_index);
}

ComponentFlags EntityManager::get_flags( EntityID const entity ) const {
    assert( entity_exists( entity ) );
    return m_component_flags.at( entity );
}

bool EntityManager::has_flags( EntityID const entity, ComponentFlags const flags ) const {
    assert( entity_exists( entity ) );
    ComponentFlags const relevant { m_component_flags.at( entity ) & flags };
    return relevant == flags;
}

EntityManager::Iterator::Iterator( EntityManager const & manager,
                                   EntityID const initial_entity,
                                   ComponentFlags const filter )
    : m_manager { manager }, m_current { initial_entity }, m_filter { filter } {}

EntityManager::Iterator & EntityManager::Iterator::operator++() {
    while ( ++m_current < g_max_entities ) {
        if ( m_manager.entity_exists( m_current ) and m_filter and m_manager.has_flags( m_current, m_filter ) )
            return *this;
    }
    return *this;
}

EntityID EntityManager::Iterator::operator*() const {
    return m_current;
}

bool EntityManager::Iterator::operator==( Iterator const & other ) const {
    assert( &m_manager == &other.m_manager );
    return m_current == other.m_current;
}

EntityManager::Iterator EntityManager::begin( ComponentFlags const filter ) const {
    return { *this, 0, filter };
}

EntityManager::Iterator EntityManager::end() const {
    return { *this, g_max_entities };
}

void EntityManager::set_flag( EntityID const entity, ComponentTypeID const component_type ) {
    assert( entity_exists( entity ) );
    assert( not has_flags( entity, id_to_flag( component_type ) ) );
    m_component_flags.at( entity ) |= id_to_flag( component_type );
}

void EntityManager::unset_flag( EntityID const entity, ComponentTypeID const component_type ) {
    assert( entity_exists( entity ) );
    assert( has_flags( entity, id_to_flag( component_type ) ) );
    m_component_flags.at( entity ) ^= id_to_flag( component_type );
}

void EntityManager::unset_all( ComponentTypeID const component_type ) {
    ComponentFlags const flag { id_to_flag( component_type ) };
    for ( auto iterator { begin( flag ) }; iterator != end(); ++iterator )
        m_component_flags.at( *iterator ) ^= flag;
}

void EntityManager::set_entity_name( std::string const & name, EntityID const id, bool const ignore_existing ) {
    if ( not ignore_existing and m_named_entities.contains( name ) )
        Log::warning( "The named entity '", name, "' is being changed to a new ID." );
    m_named_entities.insert_or_assign( name, id );
}

void EntityManager::remove_name( std::string const & name ) {
    assert( name_exists( name ) );
    m_named_entities.erase( name );
}

bool EntityManager::name_exists( std::string const & name ) const {
    return m_named_entities.contains( name );
}

EntityID EntityManager::get_entity_id( std::string const & name ) const {
    assert( name_exists( name ) );
    return m_named_entities.at( name );
}
