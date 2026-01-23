#include "component_manager.hpp"

#include <bit>
#include <cassert>
#include <stdexcept>


ComponentStore * ComponentManager::get_component_store( ComponentFlag const flag ) const {
    assert( flag_exists( flag ) );
    for ( auto const & [type_id, stored_flag] : m_component_types ) {
        if ( flag == stored_flag )
            return m_stores.at( type_id ).get();
    }
    throw std::runtime_error( "A ComponentFlag marked as in use could not actually be found." );
}

bool ComponentManager::flag_exists( ComponentFlag const flag ) const {
    assert( std::popcount( flag ) == 1 );
    return m_used_flags & (1 << flag);
}

void ComponentManager::remove_component( Entity const entity, ComponentFlag const flag ) {
    get_component_store( flag )->remove( entity );
}
