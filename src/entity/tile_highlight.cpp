#include "tile_highlight.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "graphics/entity_component_system.hpp"


Mesh<ColourVertex> create_mesh() {
    glm::vec3 constexpr highlight_colour { 0.f, 1.f, 1.f };
    glm::vec3 constexpr normal { 0.f, 1.f, 0.f };
    glm::vec3 constexpr y_offset { 0.f, 0.01f, 0.f };

    std::vector<glm::vec3> positions { tile_position( 0, 0 ), tile_position( 0, 1 ), tile_position( 1, 1 ) };
    float constexpr corner_weight { 0.8f };
    float constexpr centre_weight { 1.f - corner_weight };
    glm::vec3 const centre { (positions[0] + positions[1] + positions[2]) / 3.f };

    std::vector<ColourVertex> vertices { positions.size() * 2 };
    for ( unsigned int index { 0 }; index < positions.size(); ++index ) {
        vertices[index] = ColourVertex { positions[index] + y_offset, normal, highlight_colour };
        glm::vec3 const inner_corner { positions[index] * corner_weight + centre * centre_weight };
        vertices[positions.size() + index] = ColourVertex { inner_corner + y_offset, normal, highlight_colour };
    }

    std::vector<unsigned int> const faces { 0, 3, 2, 5, 1, 4, 0, 3 };

    Mesh<ColourVertex> result { vertices, faces, GL_TRIANGLE_STRIP };
    result.set_flag( IsLightSource ); // Disables lighting computations and sets the colour as bright as possible

    return result;
}

TileHighlight::TileHighlight( ECS * const ecs ) : Entity { ecs } {
    static Mesh<ColourVertex> mesh { create_mesh() };
    glm::vec3 constexpr position { 0.f };

    ecs->components.insert_component( m_id, Location { .position = position } );
    ecs->components.insert_component( m_id, Drawable { .mesh = &mesh } );
    ecs->components.insert_component( m_id, TerrainTile { .tile_id = position_to_tile( position ) } );
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
