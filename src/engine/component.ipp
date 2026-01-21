#ifndef DEMO_TD_COMPONENT_IPP
#define DEMO_TD_COMPONENT_IPP


#include <bit>


template <SubComponent ComponentType>
ComponentArray<ComponentType>::ComponentArray() : m_components {}, m_entities {}, m_entity_to_component {},
                                                  m_nr_components { 0 } {}

template <SubComponent ComponentType>
void ComponentArray<ComponentType>::insert( Entity const entity ) {
    insert( entity, ComponentType {} );
}

template <SubComponent ComponentType>
void ComponentArray<ComponentType>::insert( Entity const entity, ComponentType const & component ) {
    assert( m_nr_components < g_max_entities );

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
constexpr std::type_index ComponentManager::type_id() {
    return { typeid( ComponentType ) };
}

template <SubComponent ComponentType>
void ComponentManager::create_store() {
    auto const type { type_id<ComponentType>() };
    if ( m_stores.contains( type ) ) {
        Log::warning( "Attempted to add two ComponentArray objects to the same manager, ignoring the second one." );
        return;
    }

    ComponentFlag const flag { 1ull << std::countr_one( m_used_flags ) };
    assert( flag != 0 ); // Fails if m_used_flags is all ones, i.e. the maximum allowed number of types is exceeded

    m_stores.emplace( type, std::make_unique<ComponentArray<ComponentType>>() );
    m_component_types.emplace( type, flag );
    m_used_flags |= flag;
}

template <SubComponent ComponentType>
void ComponentManager::remove_store() {
    auto const type { type_id<ComponentType>() };
    if ( not m_stores.contains( type ) ) {
        Log::warning( "Attempted to remove a ComponentArray object that can't be found, removing nothing." );
        return;
    }

    m_used_flags &= ~m_component_types.at( type );
    m_component_types.erase( type );
    m_stores.erase( type );
}

template <SubComponent ComponentType>
ComponentType & ComponentManager::get_component( Entity const entity ) const {
    return get_component_array<ComponentType>().get( entity );
}

template <SubComponent ComponentType>
ComponentArray<ComponentType> & ComponentManager::get_component_array() const {
    auto const iterator { m_stores.find( type_id<ComponentType>() ) };
    assert( iterator != m_stores.end() );
    return *dynamic_cast<ComponentArray<ComponentType> *>( iterator->second.get() );
}

template <SubComponent ComponentType>
ComponentFlag ComponentManager::get_component_flag() const {
    return m_component_types.at( type_id<ComponentType>() );
}

template <SubComponent ComponentType>
void ComponentManager::insert_component( Entity const entity ) {
    get_component_array<ComponentType>().insert( entity );
}

template <SubComponent ComponentType>
void ComponentManager::remove_component( Entity const entity ) {
    get_component_array<ComponentType>().remove( entity );
}


#endif //DEMO_TD_COMPONENT_IPP
