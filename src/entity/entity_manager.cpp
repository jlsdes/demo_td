#include "entity_manager.hpp"
#include "utils/log.hpp"

#include <bit>
#include <cassert>
#include <iomanip>


EntityManager::EntityManager() : m_existing { 0 }, m_nr_entities { 0 }, m_next_entity { 0 } {}

inline std::pair<unsigned int, unsigned int> get_bit_location( Entity const entity, unsigned int const segment_size ) {
    return { entity / segment_size, entity % segment_size };
}

inline bool get_bit( unsigned long long const segment, unsigned int const bit_index ) {
    return segment & (1ull << bit_index);
}

Entity EntityManager::create() {
    assert( m_nr_entities < g_max_entities );

    auto [segment_index, bit_index] = get_bit_location( m_next_entity, s_segment_size );
    SegmentType * segment { m_existing.data() + segment_index };

    // Check whether an entity with the m_next_entity ID already exists, and if so find a different ID that is available
    if ( get_bit( *segment, bit_index ) ) {
        // Full segments can quickly be skipped
        while ( *segment == ~0ull ) {
            ++segment_index;
            if ( segment_index < s_array_size )
                ++segment;
            else {
                segment_index = 0;
                segment = m_existing.data();
            }
        }
        // Get the first open spot within this segment
        bit_index = std::countr_one( *segment );
        m_next_entity = segment_index * s_segment_size + bit_index;
    }
    *segment |= 1ull << bit_index;
    ++m_nr_entities;

    m_next_entity = (m_next_entity + 1) % g_max_entities;
    return m_next_entity;
}

void EntityManager::remove( Entity const entity ) {
    if ( not entity_exists( entity ) ) {
        Log::warning( "Attempted to remove an entity ", entity, " that does not exist, removing nothing." );
        return;
    }
    auto const [segment_index, bit_index] { get_bit_location( entity, s_segment_size ) };
    m_existing.at( segment_index ) &= ~(1ull << bit_index);
    --m_nr_entities;
}

bool EntityManager::entity_exists( Entity const entity ) const {
    auto const [segment_index, bit_index] { get_bit_location( entity, s_segment_size ) };
    return get_bit( m_existing.at( segment_index ), bit_index );
}

EntityManager::Iterator::Iterator( EntityManager & manager, Entity const initial_entity )
    : m_manager { manager }, m_current { initial_entity } {}

EntityManager::Iterator & EntityManager::Iterator::operator++() {
    while ( ++m_current < g_max_entities ) {
        if ( m_manager.entity_exists( m_current ) ) // Could be slow
            return *this;
    }
    return *this;
}

Entity EntityManager::Iterator::operator*() const {
    return m_current;
}

bool EntityManager::Iterator::operator==( Iterator const & other ) const {
    assert( &m_manager == &other.m_manager );
    return m_current == other.m_current;
}

EntityManager::Iterator EntityManager::begin() {
    return { *this, 0 };
}

EntityManager::Iterator EntityManager::end() {
    return { *this, g_max_entities };
}
