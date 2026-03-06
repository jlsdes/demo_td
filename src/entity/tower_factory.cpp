#include "tower_factory.hpp"
#include "graphics/entity_component_system.hpp"
#include "component/entity_type.hpp"
#include "component/location.hpp"


TowerFactory::TowerFactory( ECS * const ecs ) : m_ecs { ecs } {}

EntityID TowerFactory::build( TowerData::Type const type, glm::vec3 const & position ) const {
    EntityID const tower { m_ecs->entities.create() };
    m_ecs->components.insert_component<TowerData>( tower, { .type = type } );
    m_ecs->components.insert_component<Location>( tower, { .position = position } );
    m_ecs->components.insert_component<EntityType>( tower, { .type_id = EntityType::Tower } );

    return tower;
}
