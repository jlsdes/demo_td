#include "tower.hpp"
#include "core/entity_component_system.hpp"
#include "component/entity_type.hpp"
#include "component/location.hpp"


EntityID Tower::make( TowerData::Type const type, glm::vec3 const & position, ECS * const ecs ) {
    EntityID const tower { ecs->entities.create() };
    ecs->components.insert_component<TowerData>( tower, { .type = type } );
    ecs->components.insert_component<Location>( tower, { .position = position } );
    ecs->components.insert_component<EntityType>( tower, { .type_id = EntityType::Tower } );
    return tower;
}
