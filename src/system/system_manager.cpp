#include "system_manager.hpp"


void SystemManager::run_group( unsigned int const group_type ) const {
    if ( group_type == Disabled )
        return;

    for ( auto const & type_id : m_groups.at( group_type ) )
        m_systems.at( type_id )->run();
}

bool SystemManager::PriorityCompare::operator()( std::type_index const lhs, std::type_index const rhs ) const {
    unsigned int const lhs_priority { systems->m_group_data.at( lhs ).second };
    unsigned int const rhs_priority { systems->m_group_data.at( rhs ).second };

    if ( lhs_priority == rhs_priority )
        return lhs < rhs;
    return lhs_priority < rhs_priority;
}
