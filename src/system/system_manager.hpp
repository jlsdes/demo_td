#ifndef DEMO_TD_SYSTEM_MANAGER_HPP
#define DEMO_TD_SYSTEM_MANAGER_HPP

#include "component/component_manager.hpp"
#include "system.hpp"

#include <memory>
#include <map>


/** The two main groups of systems; other values can be used as well. */
enum SystemGroup : unsigned int {
    General = 0,
    Render = 1,
};


/** Manages all systems. */
class SystemManager {
public:
    explicit SystemManager( ComponentManager * component_manager );
    ~SystemManager() = default;

    SystemManager( SystemManager const & ) = delete;
    SystemManager & operator=( SystemManager const & ) = delete;

    SystemManager( SystemManager && ) = default;
    SystemManager & operator=( SystemManager && ) = default;

    template <SubSystem SystemType>
    void insert_system( ComponentFlag flags, unsigned int group_type = General );
    template <SubSystem SystemType>
    void remove_system();

    /** Updates the group the system belongs to. Systems can only belong to one group at a time. */
    template <SubSystem SystemType>
    void set_group( unsigned int group_type );

    /** Runs all systems in the given group. */
    void run_group( unsigned int group_type ) const;

private:
    std::map<std::type_index, std::unique_ptr<System>> m_systems;
    std::map<std::type_index, unsigned int> m_groups;

    ComponentManager * m_component_manager;
};


// Template definitions
#include "system_manager.ipp"


#endif //DEMO_TD_SYSTEM_MANAGER_HPP
