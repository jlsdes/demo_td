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

glm::vec3 constexpr bottom_left { 0.f, 0.f, 0.f };
glm::vec3 constexpr bottom_right { unskew( 1.f, 0.f ) };
glm::vec3 constexpr top_left { unskew( 0.f, 1.f ) };
glm::vec3 constexpr top_right { unskew( 1.f, 1.f ) };

glm::vec3 constexpr main_colour { 0.5f, 0.5f, 0.5f };
glm::vec3 constexpr border_colour { 0.2f, 0.2f, 0.2f };

MeshBuilder create_tile_builder( std::vector<glm::vec3> vertices ) {
    glm::vec3 constexpr y_offset { 0.f, 0.001f, 0.f };
    vertices.emplace_back( 0.9f * vertices.at( 0 ) + 0.05f * vertices.at( 1 ) + 0.05f * vertices.at( 2 ) + y_offset );
    vertices.emplace_back( 0.9f * vertices.at( 1 ) + 0.05f * vertices.at( 2 ) + 0.05f * vertices.at( 0 ) + y_offset );
    vertices.emplace_back( 0.9f * vertices.at( 2 ) + 0.05f * vertices.at( 0 ) + 0.05f * vertices.at( 1 ) + y_offset );
    std::vector<std::vector<unsigned int>> const faces { { 0, 1, 2 }, { 3, 4, 5 } };
    std::vector const colours { border_colour, border_colour, border_colour, main_colour, main_colour, main_colour };
    return MeshBuilder( vertices, faces, {}, colours ).generate_face_normals();
}

EntityID TileFactory::build( SkewedCoordinate const tile_id ) const {
    static std::array<Mesh<ColourVertex>, 2> meshes {
        create_tile_builder( { bottom_left, top_left, top_right } ).get_mesh(),
        create_tile_builder( { bottom_left, top_right, bottom_right } ).get_mesh()
    };

    float const x { static_cast<float>(tile_id.x) };
    float const y { static_cast<float>(tile_id.y) };
    unsigned int const half { static_cast<unsigned int>(tile_id.half ? 1 : 0) };

    EntityID const entity { m_ecs->entities.create() };
    m_ecs->components.insert_component<Drawable>( entity, { .mesh = &meshes.at( half ) } );
    m_ecs->components.insert_component<Position>( entity, { .position = unskew( x, y ) } );
    return entity;
}

std::array<EntityID, g_chunk_size> TileFactory::build_chunk( SkewedCoordinate const chunk_id ) const {
    std::array<EntityID, g_chunk_size> entities {};
    unsigned int index { 0 };

    SkewedCoordinate const base_id { chunk_id.x * 16, chunk_id.y * 16, chunk_id.half };

    // If chunk_id.half == 0, then grow towards (16, 0) and (16, 16)
    // If chunk_id.half == -1, then grow towards (0, 16) and (16, 16)

    for ( int offset_1 { 0 }; offset_1 < 16; ++offset_1 ) {
        entities.at( index++ ) = build( { base_id.x + offset_1, base_id.y + offset_1, base_id.half } );

        for ( int offset_2 { offset_1 + 1 }; offset_2 < 16; ++offset_2 ) {
            if ( base_id.half ) {
                entities.at( index++ ) = build( { base_id.x + offset_2, base_id.y + offset_1, 0 } );
                entities.at( index++ ) = build( { base_id.x + offset_2, base_id.y + offset_1, -1 } );
            } else {
                entities.at( index++ ) = build( { base_id.x + offset_1, base_id.y + offset_2, 0 } );
                entities.at( index++ ) = build( { base_id.x + offset_1, base_id.y + offset_2, -1 } );
            }
        }
    }

    return entities;
}
