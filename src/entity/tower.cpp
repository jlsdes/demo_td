#include "tower.hpp"

#include "component/component_manager.hpp"
#include "component/entity_type.hpp"
#include "component/location.hpp"
#include "core/context.hpp"
#include "entity/entity_manager.hpp"


EntityID Tower::make( TowerType::Type const type, glm::vec3 const & position, Context const & context ) {
    EntityID const tower { context.entities->create() };
    context.components->insert_component<TowerType>( tower, { .type = type } );
    context.components->insert_component<Location>( tower, { .position = position } );
    context.components->insert_component<EntityType>( tower, { .type_id = EntityType::Tower } );
    return tower;
}
