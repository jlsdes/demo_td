#include "system_manager.hpp"
#include "engine/entity_component_system.hpp"


SystemManager::SystemManager( ECS * const ecs ) : m_entities { ecs->entities }, m_components { ecs->components },
                                                  m_systems {}, m_groups {} {}

void SystemManager::run_group( unsigned int const group_type ) const {
    for ( auto const [type_id, group_id] : m_groups ) {
        if ( group_id == group_type )
            m_systems.at( type_id )->run( m_entities, m_components );
    }
}
