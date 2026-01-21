#include "entity_manager.hpp"

#include <bit>
#include <cassert>


EntityManager::EntityManager() : m_component_flags { 0 }, m_existing { 0 }, m_nr_entities { 0 }, m_next_entity { 0 } {}


Entity EntityManager::create( ComponentFlag const flags ) {
    assert( m_nr_entities < g_max_entities );

    unsigned int segment_index { m_next_entity / s_segment_size };
    SegmentType & segment { m_existing[segment_index] };
    unsigned int value_index { m_next_entity % s_segment_size };
    SegmentType mask { 1ull << value_index };

    // Check whether an entity with this ID already exists
    if ( segment & mask ) {

        while ( segment == ~0ull ) {
            // Skip full segments
            segment_index = (segment_index + 1) % s_array_size;
            segment = m_existing[segment_index];
        }
        // Get the first open spot within this segment, which could be before the previous entity in the same segment
        value_index = std::countr_one( segment );
        mask = 1ull << value_index;
        m_next_entity = segment_index * s_segment_size + value_index;
    }

    m_component_flags.at( m_next_entity ) = flags;
    segment |= mask;
    ++m_nr_entities;

    return m_next_entity++;
}

void EntityManager::remove( Entity const entity ) {}

bool EntityManager::entity_exists( Entity const entity ) const {}

ComponentFlag EntityManager::get_flags( Entity const entity ) const {}

bool EntityManager::entity_has_component( Entity const entity, ComponentFlag const flag ) const {}
