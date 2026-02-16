#include "tile_highlight.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "graphics/entity_component_system.hpp"


TileHighlight::TileHighlight( ECS * const ecs ) : Entity { ecs } {
    ecs->components.insert_component( m_id, Location {} );
    ecs->components.insert_component( m_id, Drawable {} );
    ecs->components.insert_component( m_id, TerrainTile {} );
}

void TileHighlight::show() const {
    auto const & drawable { m_ecs->components.get_component<Drawable>( m_id ) };
    drawable.mesh->unset_flag( IsHidden );
}

void TileHighlight::hide() const {
    auto const & drawable { m_ecs->components.get_component<Drawable>( m_id ) };
    drawable.mesh->set_flag( IsHidden );
}

void TileHighlight::set_position( glm::vec3 const & position ) const {
    auto & location { m_ecs->components.get_component<Location>( m_id ) };
    auto & tile { m_ecs->components.get_component<TerrainTile>( m_id ) };

    location.position = position;
    tile.tile_id = position_to_tile( position.x, position.z );
}
