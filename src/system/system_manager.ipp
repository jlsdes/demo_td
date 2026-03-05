#ifndef DEMO_TD_SYSTEM_MANAGER_IPP
#define DEMO_TD_SYSTEM_MANAGER_IPP

#include "utils/log.hpp"

#include <stdexcept>


template <SubSystem SystemType>
void SystemManager::insert_system( std::unique_ptr<SystemType> && system,
                                   unsigned int const group_type,
                                   unsigned int priority ) {
    std::type_index const type { typeid( SystemType ) };
    if ( m_systems.contains( type ) ) {
        Log::warning( "Attempted to insert system ", typeid( SystemType ).name(), " twice, ignoring duplicate." );
        return;
    }

    // If no priority value is given, take the last system's priority and add 10, or just 10 if there are no systems yet
    if ( priority == 0 ) {
        priority = 10;
        if ( m_groups.contains( group_type ) ) {
            auto const last_system { m_groups.at( group_type ).crbegin() };
            priority += m_group_data.at( *last_system ).second;
        }
    }

    if ( not m_groups.contains( group_type ) )
        m_groups.emplace( group_type, PriorityCompare { this } );

    m_systems.emplace( type, std::move( system ) );
    m_group_data.emplace( type, Data { group_type, priority } );
    m_groups.at( group_type ).emplace( type );

    Log::info( "Registered new system ", typeid( SystemType ).name(), " to group ", group_type, " with priority ",
               priority, "." );
}

template <SubSystem SystemType>
void SystemManager::remove_system() {
    std::type_index const type { typeid( SystemType ) };
    if ( not m_systems.contains( type ) ) {
        Log::warning( "Attempted to remove a system ", typeid( SystemType ).name(), " that doesn't exist, ignoring." );
        return;
    }

    auto const group { m_group_data.at( type ).first };
    m_systems.erase( type );
    m_group_data.erase( type );
    m_groups.at( group ).erase( type );
}

template <SubSystem SystemType>
SystemType * SystemManager::get_system() {
    std::type_index const type { typeid( SystemType ) };
    if ( not m_systems.contains( type ) ) {
        std::string constexpr name { typeid( SystemType ).name() };
        throw std::logic_error( std::format( "Attempted to access a system {} that wasn't registered.", name ) );
    }

    return m_systems.at( type ).get();
}

template <SubSystem SystemType>
void SystemManager::set_group( unsigned int const group_type, unsigned int priority ) {
    std::type_index const type { typeid( SystemType ) };
    if ( not m_systems.contains( type ) ) {
        Log::error( "Attempted to set the group of a system ", typeid( SystemType ).name(), " that isn't registered." );
        return;
    }

    auto const [old_group, old_priority] { m_group_data.at( type ) };

    if ( priority == 0 )
        priority = old_priority;
    if ( not m_groups.contains( group_type ) )
        m_groups.emplace( group_type, PriorityCompare { this } );

    m_groups.at( old_group ).erase( type );
    m_groups.at( group_type ).emplace( type );
    m_group_data.at( type ) = { group_type, priority };
}

#endif //DEMO_TD_SYSTEM_MANAGER_IPP
