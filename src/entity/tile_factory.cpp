#include "tile_factory.hpp"
#include "component/drawable.hpp"
#include "component/position.hpp"
#include "graphics/entity_component_system.hpp"
#include "graphics/mesh.hpp"
#include "graphics/mesh_builder.hpp"

#include <numbers>
#include <vector>

#include <glm/glm.hpp>


TileFactory::TileFactory( ECS * const ecs ) : m_ecs { ecs } {}

auto constexpr F { 0.5f * (std::numbers::sqrt3_v<float> - 1) };
auto constexpr G { 0.5f - std::numbers::sqrt3_v<float> / 6.f };

constexpr glm::vec3 skew( float const x, float const y ) {
    float const skewing_factor { F * (x + y) };
    return { x + skewing_factor, 0.f, y + skewing_factor };
}

constexpr glm::vec3 unskew( float const x, float const y ) {
    float const unskewing_factor { G * (x + y) };
    return { x - unskewing_factor, 0.f, y - unskewing_factor };
}

constexpr glm::vec3 unskew( int const x, int const y ) {
    return unskew( static_cast<float>(x), static_cast<float>(y) );
}

glm::vec3 constexpr bottom_left { unskew( 0, 0 ) };
glm::vec3 constexpr bottom_right { unskew( 1, 0 ) };
glm::vec3 constexpr top_left { unskew( 0, 1 ) };
glm::vec3 constexpr top_right { unskew( 1, 1 ) };

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
    static std::array<Mesh<ColourVertex>, 2> meshes {
        create_tile_builder( { bottom_left, top_left, top_right } ).get_mesh(),
        create_tile_builder( { bottom_left, top_right, bottom_right } ).get_mesh()
    };

    EntityID const entity { m_ecs->entities.create() };
    m_ecs->components.insert_component<Drawable>( entity, { .mesh = &meshes.at( tile_id.half ) } );
    m_ecs->components.insert_component<Position>( entity, { .position = unskew( tile_id.x, tile_id.y ) } );
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
