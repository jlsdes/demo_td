#include "component_manager.hpp"

#include <bit>
#include <cassert>


ComponentStore::~ComponentStore() = default;

ComponentManager::ComponentManager() : m_stores {}, m_entity_flags { 0 }, m_used_flags { 0 } {}

void ComponentManager::remove_store( ComponentTypeID const id ) {
    assert( id < g_max_component_types );
    auto const & store { m_stores.at( id ).store };
    if ( not store ) {
        Log::warning( "Attempted to remove a ComponentArray object that can't be found, removing nothing." );
        return;
    }
    // Remove any components still contained within the store
    // TODO refactor this maybe, iterating over the entities like this doesn't seem entirely correct
    ComponentFlags const flag { id_to_flag( id ) };
    for ( Entity entity { 0 }; entity < g_max_entities; ++entity ) {
        if ( m_entity_flags.at( entity ) & flag )
            remove_component( entity, id );
    }

    m_stores.at( id ) = { nullptr, nullptr };
    m_used_flags &= ~flag;
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

void ComponentManager::remove_entity( Entity const entity ) {
    ComponentFlags const flags { m_entity_flags.at( entity ) };
    ComponentFlags current { 1 };
    for ( ComponentTypeID id { 0 }; id < g_max_component_types; ++id ) {
        if ( flags & current )
            remove_component( entity, id );
        current <<= 1;
    }
}
