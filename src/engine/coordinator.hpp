#ifndef DEMO_TD_GENERAL_MANAGER_HPP
#define DEMO_TD_GENERAL_MANAGER_HPP

#include "component/component_manager.hpp"
#include "entity/entity_manager.hpp"
#include "system/system_manager.hpp"


/** Manages an entire Entity-Component-System cluster. */
class Coordinator {
public:
    Coordinator();
    ~Coordinator() = default;

    Coordinator( Coordinator const & ) = delete;
    Coordinator & operator=( Coordinator const & ) = delete;

    Coordinator( Coordinator && ) = default;
    Coordinator & operator=( Coordinator && ) = default;

    /** Adds a new component type, and returns the new representative component flag. */
    template <SubComponent ComponentType>
    ComponentFlags insert_component_type();
    /** Removes the component type, after removing all existing components of that type. */
    template <SubComponent ComponentType>
    void remove_component_type();

    template <SubComponent ComponentType>
    [[nodiscard]] ComponentFlags get_component_flag() const;

    template <SubComponent ComponentType>
    void insert_component( Entity entity, ComponentType const & component );
    template <SubComponent ComponentType>
    void remove_component( Entity entity );

    template <SubSystem SystemType>
    void insert_system( ComponentFlags required_components, unsigned int group = SystemGroup::General );
    template <SubSystem SystemType>
    void remove_system();

    /** Runs a group of systems. 'group' can be any number, but if no system has that group number, nothing happens. */
    void run_systems( unsigned int group ) const;

    Entity insert_entity();
    void remove_entity( Entity entity );

private:
    ComponentManager m_components;
    EntityManager m_entities;
    SystemManager m_systems;
};


// Template definitions
#include "coordinator.ipp"


#endif //DEMO_TD_GENERAL_MANAGER_HPP