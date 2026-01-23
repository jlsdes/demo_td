#ifndef DEMO_TD_SYSTEM_MANAGER_IPP
#define DEMO_TD_SYSTEM_MANAGER_IPP

#include "utils/log.hpp"


template <SubSystem SystemType>
void SystemManager::insert_system( ComponentFlags const flags, unsigned int group_type ) {
    std::type_index const type { typeid( SystemType ) };
    if ( m_systems.contains( type ) ) {
        Log::warning( "Attempted to insert system ", typeid( SystemType ).name(), " twice, ignoring duplicate." );
        return;
    }
    m_groups.emplace( type, group_type );
    m_systems.emplace( type, std::make_unique<SystemType>( flags, m_component_manager ) );
}

template <SubSystem SystemType>
void SystemManager::remove_system() {
    std::type_index const type { typeid( SystemType ) };
    if ( not m_systems.contains( type ) ) {
        Log::warning( "Attempted to remove a system ", typeid( SystemType ).name(), " that doesn't exist, ignoring." );
        return;
    }
    m_systems.erase( type );
    m_groups.erase( type );
}

template <SubSystem SystemType>
void SystemManager::set_group( unsigned int const group_type ) {
    std::type_index const type { typeid( SystemType ) };
    m_groups.at( type ) = group_type;
}

#endif //DEMO_TD_SYSTEM_MANAGER_IPP
