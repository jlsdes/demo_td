#ifndef DEMO_TD_SYSTEM_MANAGER_HPP
#define DEMO_TD_SYSTEM_MANAGER_HPP

#include "system.hpp"

#include <memory>
#include <map>
#include <set>
#include <typeindex>


struct Context;


/** The two main groups of systems; other values can be used as well. */
enum SystemGroup : unsigned int {
    Disabled,
    General,
    Render,
    Setup,
};


/** Manages all systems. */
class SystemManager {
public:
    SystemManager() = default;
    ~SystemManager() = default;

    SystemManager( SystemManager const & ) = delete;
    SystemManager & operator=( SystemManager const & ) = delete;

    SystemManager( SystemManager && ) = default;
    SystemManager & operator=( SystemManager && ) = delete;

    /** Adds a new system to the ECS. Only one system of each type can be added at once. If no argument (or a null
     *  argument) is provided, then a constructor that accepts only an ECS object pointer will be called. If a priority
     *  value is omitted (or is 0), then a priority value will be selected such that the system runs after every other
     *  currently registered system. */
    template <SubSystem SystemType>
    void insert_system( std::unique_ptr<SystemType> && system = nullptr,
                        unsigned int group_type = General,
                        unsigned int priority = 0 );
    /** Removes the system with the given type from the ECS. */
    template <SubSystem SystemType>
    void remove_system();

    /** Returns the registered system of the given type. */
    template <SubSystem SystemType>
    SystemType * get_system();

    /** Updates the group the system belongs to. Systems can only belong to one group at a time. The priority values
     *  determine the order in which systems of the same group are run, where systems with lower priority values are
     *  run before systems with higher priority values. If the priority value is missing or 0, then the old priority
     *  value is kept instead. */
    template <SubSystem SystemType>
    void set_group( unsigned int group_type, unsigned int priority = 0 );

    /** Runs all systems in the given group. */
    void run_group( unsigned int group_type ) const;

private:
    /// All systems that are currently registered.
    std::map<std::type_index, std::unique_ptr<System>> m_systems;

    /// A mapping from the system types to their current group IDs and priority values.
    using Data = std::pair<unsigned int, unsigned int>;
    std::map<std::type_index, Data> m_group_data;

    /// Used by std::set to sort the systems within a group according to their priority values.
    struct PriorityCompare {
        SystemManager const * const systems;
        [[nodiscard]] bool operator()( std::type_index lhs, std::type_index rhs ) const;
    };

    /// A mapping of group IDs to the systems registered to that group, preceded by their respective priority values.
    using GroupSet = std::set<std::type_index, PriorityCompare>;
    std::map<unsigned int, GroupSet> m_groups;
};


// Template definitions
#include "system_manager.ipp"


#endif //DEMO_TD_SYSTEM_MANAGER_HPP
