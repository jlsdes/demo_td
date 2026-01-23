#include "system_manager.hpp"


SystemManager::SystemManager( ComponentManager * const component_manager )
    : m_systems {}, m_groups {}, m_component_manager { component_manager } {}

void SystemManager::run_group( unsigned int const group_type ) const {
    for ( auto const [type_id, group_id] : m_groups ) {
        if ( group_id == group_type )
            m_systems.at( type_id )->run();
    }
}
