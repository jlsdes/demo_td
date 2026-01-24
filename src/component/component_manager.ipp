#ifndef DEMO_TD_COMPONENT_IPP
#define DEMO_TD_COMPONENT_IPP

#include <bit>

#include "engine/entity_component_system.hpp"


template <SubComponent ComponentType>
ComponentArray<ComponentType>::ComponentArray() : ComponentStore {}, m_components {}, m_component_to_entity {},
                                                  m_entity_to_component {}, m_nr_components { 0 } {}

template <SubComponent ComponentType>
void ComponentArray<ComponentType>::insert( Entity const entity ) {
    insert( entity, ComponentType {} );
}

template <SubComponent ComponentType>
void ComponentArray<ComponentType>::insert( Entity const entity, ComponentType const & component ) {
    assert( m_nr_components < g_max_entities );

    if ( m_entity_to_component.contains( entity ) ) {
        Log::error( "Attempted to add two ", typeid( ComponentType ).name(), " components to one entity ", entity,
                    ", skipping duplicate." );
        return;
    }
    m_components.at( m_nr_components ) = component;
    m_component_to_entity.at( m_nr_components ) = entity;
    m_entity_to_component.emplace( entity, m_nr_components );
    ++m_nr_components;
}

template <SubComponent ComponentType>
void ComponentArray<ComponentType>::remove( Entity const entity ) {
    auto const iterator { m_entity_to_component.find( entity ) };
    if ( iterator == m_entity_to_component.end() ) {
        auto const name { typeid( ComponentType ).name() };
        Log::warning( "Attempted to remove a ", name, " component of entity ", entity, ", but it could not be found." );
        return;
    }

    // Swap this component with the last one in the array (if necessary), and then just say that the array is smaller
    unsigned int const index { iterator->second };
    unsigned int const last_index { m_nr_components - 1 };
    if ( index != last_index ) {
        Entity const last_entity { m_component_to_entity.at( last_index ) };
        m_components.at( index ) = m_components.at( last_index );
        m_component_to_entity.at( index ) = last_entity;
        m_entity_to_component.at( last_entity ) = index;
    }
    m_entity_to_component.erase( iterator );
    --m_nr_components;
}

template <SubComponent ComponentType>
ComponentType & ComponentArray<ComponentType>::get( Entity const entity ) {
    return m_components.at( m_entity_to_component.at( entity ) );
}

template <SubComponent ComponentType>
ComponentType * ComponentArray<ComponentType>::begin() {
    return m_components.begin();
}

template <SubComponent ComponentType>
ComponentType * ComponentArray<ComponentType>::end() {
    return m_components.begin() + m_nr_components;
}

template <SubComponent ComponentType>
unsigned int ComponentArray<ComponentType>::size() const {
    return m_nr_components;
}

template <SubComponent ComponentType>
bool ComponentArray<ComponentType>::empty() const {
    return m_nr_components == 0;
}

template <SubComponent ComponentType>
bool ComponentArray<ComponentType>::contains( Entity const entity ) const {
    return m_entity_to_component.contains( entity );
}

template <SubComponent ComponentType>
ComponentTypeID ComponentManager::create_store() {
    std::type_index const type { typeid( ComponentType ) };
    if ( m_types.contains( type ) ) {
        Log::warning( "Attempted to create a component store with the same type ", typeid( ComponentType ).name(),
                      " twice, ignoring duplicate." );
        return m_types.at( type );
    }
    ComponentTypeID const type_id { static_cast<unsigned char>(std::countr_one( m_used_flags )) };
    assert( type_id < g_max_component_types );

    m_stores.at( type_id ) = std::make_unique<ComponentArray<ComponentType>>();
    m_types.emplace( type, type_id );
    return type_id;
}

template <SubComponent ComponentType>
ComponentTypeID ComponentManager::get_type_id() const {
    return m_types.at( std::type_index { typeid( ComponentType ) } );
}

template <SubComponent ComponentType>
void ComponentManager::insert_component( Entity const entity, ComponentType const & component ) {
    std::type_index const type { typeid( ComponentType ) };
    if ( not m_types.contains( type ) ) {
        Log::error( "Attempted to insert a component of an unregistered type ", typeid( ComponentType ).name(),
                    ", ignoring." );
        return;
    }
    ComponentTypeID const type_id { m_types.at( type ) };
    auto const store { m_stores.at( type_id ).get() };
    auto const array { dynamic_cast<ComponentArray<ComponentType> *>(store) };
    array->insert( entity, component );
    m_entities.set_flag( entity, type_id );
}

template <SubComponent ComponentType>
ComponentType * ComponentManager::begin() const {
    ComponentTypeID const type_id { m_types.at( typeid( ComponentType ) ) };
    auto const array { dynamic_cast<ComponentArray<ComponentType> *>(m_stores.at( type_id ).get()) };
    return array->begin();
}

template <SubComponent ComponentType>
ComponentType * ComponentManager::end() const {
    ComponentTypeID const type_id { m_types.at( typeid( ComponentType ) ) };
    auto const array { dynamic_cast<ComponentArray<ComponentType> *>(m_stores.at( type_id ).get()) };
    return array->begin();
}


#endif //DEMO_TD_COMPONENT_IPP
