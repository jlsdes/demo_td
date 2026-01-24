#ifndef DEMO_TD_SYSTEM_MANAGER_HPP
#define DEMO_TD_SYSTEM_MANAGER_HPP

#include "system.hpp"

#include <memory>
#include <map>
#include <typeindex>


struct ECS;
class EntityManager;
class ComponentManager;


/** The two main groups of systems; other values can be used as well. */
enum SystemGroup : unsigned int {
    General = 0,
    Render = 1,
};


/** Manages all systems. */
class SystemManager {
public:
    explicit SystemManager( ECS * ecs );
    ~SystemManager() = default;

    SystemManager( SystemManager const & ) = delete;
    SystemManager & operator=( SystemManager const & ) = delete;

    SystemManager( SystemManager && ) = default;
    SystemManager & operator=( SystemManager && ) = delete;

    template <SubSystem SystemType>
    void insert_system( ComponentFlags flags, unsigned int group_type = General );
    template <SubSystem SystemType>
    void remove_system();

    /** Updates the group the system belongs to. Systems can only belong to one group at a time. */
    template <SubSystem SystemType>
    void set_group( unsigned int group_type );

    /** Runs all systems in the given group. */
    void run_group( unsigned int group_type ) const;

private:
    /// The ECS object contains this object, and the partnered EntityManager and ComponentManager objects.
    EntityManager & m_entities;
    ComponentManager & m_components;

    std::map<std::type_index, std::unique_ptr<System>> m_systems;
    std::map<std::type_index, unsigned int> m_groups;
};


// Template definitions
#include "system_manager.ipp"


#endif //DEMO_TD_SYSTEM_MANAGER_HPP
