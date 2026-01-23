#ifndef DEMO_TD_COORDINATOR_IPP
#define DEMO_TD_COORDINATOR_IPP


template <SubComponent ComponentType>
ComponentFlag Coordinator::insert_component_type() {
    m_components.create_store<ComponentType>();
    return m_components.get_component_flag<ComponentType>();
}

template <SubComponent ComponentType>
void Coordinator::remove_component_type( bool const purge ) {
    if ( purge ) {
        ComponentFlag const flag { m_components.get_component_flag<ComponentType>() };
        for ( auto iterator { m_entities.begin( flag ) }; iterator != m_entities.end(); ++iterator )
            m_components.remove_component<ComponentFlag>( *iterator );
    }
    m_components.remove_store<ComponentType>();
}

template <SubComponent ComponentType>
ComponentFlag Coordinator::get_component_flag() const {
    return m_components.get_component_flag<ComponentType>();
}

template <SubComponent ComponentType>
void Coordinator::insert_component( Entity const entity, ComponentType const & component ) {
    ComponentFlag const flag { m_components.get_component_flag<ComponentType>() };
    assert( not m_entities.entity_has_components( entity, flag ) );

    m_components.insert_component<ComponentType>( entity, component );
    m_entities.toggle_flags( entity, flag );

}

template <SubComponent ComponentType>
void Coordinator::remove_component( Entity const entity ) {
    ComponentFlag const flag { m_components.get_component_flag<ComponentType>() };
    assert( m_entities.entity_has_components( entity, flag ) );

    m_components.remove_component<ComponentType>(entity);
    m_entities.toggle_flags( entity, flag );
}

template <SubSystem SystemType>
void Coordinator::insert_system( ComponentFlag const required_components, unsigned int const group ) {
    m_systems.insert_system<SystemType>( required_components, group );
}

template <SubSystem SystemType>
void Coordinator::remove_system() {
    m_systems.remove_system<SystemType>();
}


#endif //DEMO_TD_COORDINATOR_IPP