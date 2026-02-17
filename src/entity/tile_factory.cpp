#include "tile_factory.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"
#include "graphics/entity_component_system.hpp"
#include "graphics/mesh.hpp"
#include "graphics/mesh_builder.hpp"

#include <numbers>
#include <vector>

#include <glm/glm.hpp>


TileFactory::TileFactory( ECS * const ecs ) : m_ecs { ecs } {}

glm::vec3 constexpr bottom_left { tile_position( 0, 0 ) };
glm::vec3 constexpr bottom_right { tile_position( 1, 0 ) };
glm::vec3 constexpr top_left { tile_position( 0, 1 ) };
glm::vec3 constexpr top_right { tile_position( 1, 1 ) };

glm::vec3 constexpr main_colour { 0.5f, 0.5f, 0.5f };
glm::vec3 constexpr border_colour { 0.2f, 0.2f, 0.2f };

/** Helper function that creates the MeshBuilder for a single tile. Each tile is (at the time of writing) composed of
 *  two triangles, with one being slightly larger than the other to create a tile border effect. */
MeshBuilder create_tile_builder( std::vector<glm::vec3> vertices ) {
    glm::vec3 constexpr y_offset { 0.f, 0.001f, 0.f };
    float constexpr main_weight { 0.95f };
    float constexpr side_weight { (1.f - main_weight) / 2.f };

    glm::vec3 const vertex_0 { vertices.at( 0 ) };
    glm::vec3 const vertex_1 { vertices.at( 1 ) };
    glm::vec3 const vertex_2 { vertices.at( 2 ) };
    vertices.emplace_back( main_weight * vertex_0 + side_weight * vertex_1 + side_weight * vertex_2 + y_offset );
    vertices.emplace_back( main_weight * vertex_1 + side_weight * vertex_2 + side_weight * vertex_0 + y_offset );
    vertices.emplace_back( main_weight * vertex_2 + side_weight * vertex_0 + side_weight * vertex_1 + y_offset );

    std::vector<std::vector<unsigned int>> const faces { { 0, 1, 2 }, { 3, 4, 5 } };
    std::vector const colours { border_colour, border_colour, border_colour, main_colour, main_colour, main_colour };
    return MeshBuilder( vertices, faces, {}, colours ).generate_face_normals();
}

EntityID TileFactory::build( SkewedCoordinate const tile_id ) const {
    static InstancedMesh mesh { create_tile_builder( { bottom_left, top_left, top_right } ).get_mesh() };

    glm::quat rotation {};
    if ( tile_id.half )
        rotation = glm::quat { glm::vec3 { 0.f, std::numbers::pi_v<float> / 3.f, 0.f } };
    glm::mat3 const orientation { glm::mat3_cast( rotation ) };

    Drawable const drawable { .mesh = &mesh, .orientation = orientation, .priority = Terrain };
    Location const position { .position = tile_position( tile_id.x, tile_id.y ) };

    EntityID const entity { m_ecs->entities.create() };
    m_ecs->components.insert_component( entity, drawable );
    m_ecs->components.insert_component( entity, position );
    return entity;
}

std::array<EntityID, g_chunk_size> TileFactory::build_chunk( SkewedCoordinate const chunk_id ) const {
    std::array<EntityID, g_chunk_size> entities {};
    unsigned int index { 0 };

    int const base_x { chunk_id.x * static_cast<int>(g_chunk_length) };
    int const base_y { chunk_id.y * static_cast<int>(g_chunk_length) };
    int const half { static_cast<int>(chunk_id.half) };

    // Iterating over all triangles by the barycentric coordinates (u, v) of their "bottom-left" corner
    // w is not used here and is thus omitted; the full barycentric coordinates are (u, v, 16-u-v)
    for ( int u { 0 }; u < g_chunk_length; ++u ) {
        entities.at( index++ ) = build( { base_x + u, base_y + u, chunk_id.half } );

        for ( int v { u + 1 }; v < g_chunk_length; ++v ) {
            int const x { base_x + (1 - half) * u + half * v };
            int const y { base_y + (1 - half) * v + half * u };
            entities.at( index++ ) = build( { x, y, 0 } );
            entities.at( index++ ) = build( { x, y, 1 } );
        }
    }

    return entities;
}
