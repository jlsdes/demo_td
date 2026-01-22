#include "entity_manager.hpp"
#include "utils/log.hpp"

#include <bit>
#include <cassert>
#include <iomanip>

#include "glm/gtc/bitfield.hpp"


EntityManager::EntityManager() : m_component_flags { 0 }, m_existing { 0 }, m_nr_entities { 0 }, m_next_entity { 0 } {}

inline std::pair<unsigned int, unsigned int> get_bit_location( Entity const entity, unsigned int const segment_size ) {
    return { entity / segment_size, entity % segment_size };
}

inline bool get_bit( unsigned long long const segment, unsigned int const bit_index ) {
    return segment & (1ull << bit_index);
}

Entity EntityManager::create( ComponentFlag const flags ) {
    assert( m_nr_entities < g_max_entities );

    auto [segment_index, bit_index] = get_bit_location( m_next_entity, s_segment_size );
    SegmentType * segment { m_existing.data() + segment_index };

    // Check whether an entity with this ID already exists
    if ( get_bit( *segment, bit_index ) ) {
        while ( *segment == ~0ull ) {
            // Skip full segments
            ++segment_index;
            if ( segment_index < s_array_size )
                ++segment;
            else {
                segment_index = 0;
                segment = m_existing.data();
            }
        }
        // Get the first open spot within this segment, which could be before the previous entity in the same segment
        bit_index = std::countr_one( *segment );
        m_next_entity = segment_index * s_segment_size + bit_index;
    }
    m_component_flags.at( m_next_entity ) = flags;
    *segment |= 1ull << bit_index;
    ++m_nr_entities;
    return m_next_entity++;
}

void EntityManager::remove( Entity const entity ) {
    if ( not entity_exists( entity ) ) {
        Log::warning( "Attempted to remove an entity ", entity, " that does not exist, removing nothing." );
        return;
    }
    auto const [segment_index, bit_index] { get_bit_location( entity, s_segment_size ) };
    m_existing.at( segment_index ) &= ~(1ull << bit_index);
    m_component_flags.at( entity ) = 0ull;
    --m_nr_entities;
}

bool EntityManager::entity_exists( Entity const entity ) const {
    auto const [segment_index, bit_index] { get_bit_location( entity, s_segment_size ) };
    return get_bit( m_existing.at( segment_index ), bit_index );
}

ComponentFlag EntityManager::get_flags( Entity const entity ) const {
    if ( not entity_exists( entity ) ) {
        Log::warning( "Attempted to get the component flags of a non-existent entity ", entity, ", returning 0." );
        return 0;
    }
    return m_component_flags.at( entity );
}

bool EntityManager::entity_has_components( Entity const entity, ComponentFlag const flags ) const {
    // There could be multiple flags set to 1, in which case a true result would indicate they're all present
    ComponentFlag const component_flags { get_flags( entity ) };
    ComponentFlag const relevant_flags { component_flags & flags };
    return relevant_flags == flags;
}
