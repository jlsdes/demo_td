#ifndef DEMO_TD_COORDINATOR_IPP
#define DEMO_TD_COORDINATOR_IPP


template <SubComponent ComponentType>
ComponentFlags Coordinator::insert_component_type() {
    ComponentTypeID const id { m_components.create_store<ComponentType>() };
    return id_to_flag( id );
}

template <SubComponent ComponentType>
void Coordinator::remove_component_type() {
    m_components.remove_store( m_components.get_type_id<ComponentType>() );
}

template <SubComponent ComponentType>
ComponentFlags Coordinator::get_component_flag() const {
    return m_components.get_type_flag<ComponentType>();
}

template <SubComponent ComponentType>
void Coordinator::insert_component( Entity const entity, ComponentType const & component ) {
    m_components.insert_component<ComponentType>( entity, component );
}

template <SubComponent ComponentType>
void Coordinator::remove_component( Entity const entity ) {
    m_components.remove_component<ComponentType>( entity );
}

template <SubSystem SystemType>
void Coordinator::insert_system( ComponentFlags const required_components, unsigned int const group ) {
    m_systems.insert_system<SystemType>( required_components, group );
}

template <SubSystem SystemType>
void Coordinator::remove_system() {
    m_systems.remove_system<SystemType>();
}


#endif //DEMO_TD_COORDINATOR_IPP
