#include "system_manager.hpp"
#include "graphics/entity_component_system.hpp"


SystemManager::SystemManager( ECS * const ecs ) : m_ecs { ecs }, m_systems {}, m_groups {} {}

void SystemManager::run_group( unsigned int const group_type ) const {
    if ( group_type == Disabled )
        return;

    for ( auto const [type_id, group_id] : m_groups ) {
        if ( group_id == group_type )
            m_systems.at( type_id )->run();
    }
}
