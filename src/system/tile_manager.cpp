#include "tile_manager.hpp"

#include "component/entity_type.hpp"
#include "component/terrain_tile.hpp"

#include "entity/entity_manager.hpp"
#include "component/component_manager.hpp"


TileManager::TileManager( Context const & context ) : System { context }, m_updated { false } {}

void TileManager::run() {}

void TileManager::add_tile( SkewedCoordinate const tile_id ) {
    EntityID const entity { m_context.entities->create() };
    m_context.components->insert_component<EntityType>( entity, { .type_id = EntityType::Tile } );
    m_context.components->insert_component<TerrainTile>( entity, { .tile_id = tile_id } );

    m_updated = true;
}

void TileManager::add_chunk( SkewedCoordinate const chunk_id ) {
    int const base_x { chunk_id.x * static_cast<int>(g_chunk_length) };
    int const base_y { chunk_id.y * static_cast<int>(g_chunk_length) };
    int const half { static_cast<int>(chunk_id.half) };

    // Iterating over all triangles by the barycentric coordinates (u, v) of their "bottom-left" corner
    // w is not used here and is thus omitted; the full barycentric coordinates are (u, v, 16-u-v)
    for ( int u { 0 }; u < g_chunk_length; ++u ) {
        add_tile( { base_x + u, base_y + u, chunk_id.half } );

        for ( int v { u + 1 }; v < g_chunk_length; ++v ) {
            int const x { base_x + (1 - half) * u + half * v };
            int const y { base_y + (1 - half) * v + half * u };
            add_tile( { x, y, 0 } );
            add_tile( { x, y, 1 } );
        }
    }
}

bool TileManager::has_updated() const {
    return m_updated;
}

void TileManager::clear_updates() {
    m_updated = false;
}
