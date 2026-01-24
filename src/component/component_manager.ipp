#ifndef DEMO_TD_COMPONENT_IPP
#define DEMO_TD_COMPONENT_IPP

#include <bit>


template <SubComponent ComponentType>
ComponentArray<ComponentType>::ComponentArray() : ComponentStore {}, m_components {}, m_entities {},
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
    m_entities.at( m_nr_components ) = entity;
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
        Entity const last_entity { m_entities.at( last_index ) };
        m_components.at( index ) = m_components.at( last_index );
        m_entities.at( index ) = last_entity;
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
bool ComponentArray<ComponentType>::contains( Entity entity ) const {
    return m_entity_to_component.contains( entity );
}


#endif //DEMO_TD_COMPONENT_IPP
