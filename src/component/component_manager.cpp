#include "component_manager.hpp"

#include <cassert>

#include "engine/entity_component_system.hpp"


ComponentManager::ComponentManager( ECS * const ecs ) : m_entities { ecs->entities }, m_systems { ecs->systems },
                                                        m_stores { nullptr }, m_types {}, m_used_flags { 0 } {}

void ComponentManager::remove_store( ComponentTypeID const type_id ) {
    assert( has_store( type_id ) );

    ComponentFlags const flag { id_to_flag( type_id ) };
    m_entities.unset_all( flag );

    for ( auto i { m_types.cbegin() }; i != m_types.cend(); ++i ) {
        if ( i->second == type_id ) {
            m_types.erase( i );
            break;
        }
    }
    m_stores.at( type_id ) = nullptr;
    m_used_flags ^= flag;
}

bool ComponentManager::has_store( ComponentTypeID const type_id ) const {
    return m_stores.at( type_id ) != nullptr;
}

void ComponentManager::remove_component( EntityID const entity, ComponentTypeID const type_id ) {
    assert( has_store( type_id ) );
    assert( entity_has_component( entity, type_id ) );

    m_stores.at( type_id )->remove( type_id );
    m_entities.unset_flag( entity, type_id );
}

bool ComponentManager::entity_has_component( EntityID const entity, ComponentTypeID const type_id ) const {
    return has_store( type_id ) and m_stores.at( type_id )->contains( entity );
}

Component & ComponentManager::get_component( EntityID const entity, ComponentTypeID const type_id ) const {
    assert( has_store(type_id) );
    assert( entity_has_component( entity, type_id ) );
    return m_stores.at( type_id )->get( entity );
}

Component * ComponentManager::begin( ComponentTypeID const type_id ) const {
    assert( has_store(type_id) );
    return m_stores.at( type_id )->begin();
}

Component * ComponentManager::end( ComponentTypeID const type_id ) const {
    assert( has_store(type_id) );
    return m_stores.at( type_id )->end();
}
