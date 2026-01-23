#ifndef DEMO_TD_GENERAL_MANAGER_HPP
#define DEMO_TD_GENERAL_MANAGER_HPP

#include "component_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager.hpp"


/** Manages an entire Entity-Component-System cluster. */
class Coordinator {
public:
    Coordinator();
    ~Coordinator() = default;

    Coordinator( Coordinator const & ) = delete;
    Coordinator & operator=( Coordinator const & ) = delete;

    Coordinator( Coordinator && ) = default;
    Coordinator & operator=( Coordinator && ) = default;

    template <SubComponent ComponentType>
    void insert_component_type();
    template <SubComponent ComponentType>
    void remove_component_type( bool purge = true );

    template <SubSystem SystemType>
    void insert_system( ComponentFlag required_components );
    template <SubSystem SystemType>
    void remove_system();

    Entity insert_entity();
    void remove_entity( Entity entity );

    template <SubComponent ComponentType>
    void insert_component( Entity entity, ComponentType const & component );
    template <SubComponent ComponentType>
    void remove_component( Entity entity );

private:
    ComponentManager m_components;
    EntityManager m_entities;
    SystemManager m_systems;
};


// Template definitions
#include "coordinator.ipp"


#endif //DEMO_TD_GENERAL_MANAGER_HPP