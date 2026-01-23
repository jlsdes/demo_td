#include "component_manager.hpp"

#include <bit>
#include <cassert>


ComponentManager::ComponentManager() : m_stores {}, m_entity_flags { 0 }, m_used_flags { 0 } {}


void ComponentManager::remove_store( ComponentTypeID const id ) {
    assert( id < g_max_component_types );
    auto const & store { m_stores.at( id ).store };
    if ( not store ) {
        Log::warning( "Attempted to remove a ComponentArray object that can't be found, removing nothing." );
        return;
    }
    // TODO set entity flags accordingly

    m_stores.at( id ) = { nullptr, nullptr };
    m_used_flags &= ~id_to_flag( id );
}

ComponentStore * ComponentManager::get_component_store( ComponentFlags const flag ) const {
    assert( std::popcount( flag ) == 1 );
    return m_stores.at( flag_to_id( flag ) ).store.get();
}

bool ComponentManager::id_exists( ComponentTypeID const id ) const {
    return m_used_flags & id_to_flag( id );
}

bool ComponentManager::flag_exists( ComponentFlags const flag ) const {
    assert( std::popcount( flag ) == 1 );
    return m_used_flags & flag;
}

Component & ComponentManager::get_component( Entity const entity, ComponentTypeID const id ) const {
    return m_stores.at( id ).store->get( entity );
}

void ComponentManager::insert_component( Entity const entity, ComponentTypeID const id ) {
    get_component_store( id )->insert( entity );
}

void ComponentManager::remove_component( Entity const entity, ComponentTypeID const id ) {
    get_component_store( id )->remove( entity );
}

void ComponentManager::remove_entity( Entity const entity ) {}
