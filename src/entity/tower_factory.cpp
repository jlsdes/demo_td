#include "tower_factory.hpp"
#include "graphics/entity_component_system.hpp"
#include "graphics/mesh_builder.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"


TowerFactory::TowerFactory( ECS * const ecs ) : m_ecs { ecs } {}

EntityID TowerFactory::build( TowerData::Type const type, glm::vec3 const & position ) const {
    glm::vec3 const & colour { s_colours.at( type ) };

    EntityID const tower { m_ecs->entities.create() };
    EntityID const crystal { m_ecs->entities.create() };

    static MeshBuilder tower_builder { MeshBuilder::cube() };
    static std::array<std::unique_ptr<Mesh<ColourVertex>>, TowerData::NumberTypes> tower_meshes { nullptr };

    std::unique_ptr<Mesh<ColourVertex>> & tower_mesh { tower_meshes.at( type ) };
    if ( not tower_mesh )
        tower_mesh = std::make_unique<Mesh<ColourVertex>>( tower_builder.colour( colour ).get_mesh() );

    glm::vec3 constexpr tower_scale { 0.3f, 0.1f, 0.3f };
    m_ecs->components.insert_component<TowerData>( tower, { .type = type } );
    m_ecs->components.insert_component<Drawable>( tower, { .mesh = tower_mesh.get(), .scale = tower_scale } );
    m_ecs->components.insert_component<Location>( tower, { .position = position } );

    static MeshBuilder crystal_builder { MeshBuilder::octahedron() };
    static std::array<std::unique_ptr<Mesh<ColourVertex>>, TowerData::NumberTypes> crystal_meshes { nullptr };

    std::unique_ptr<Mesh<ColourVertex>> & crystal_mesh { crystal_meshes.at( type ) };
    if ( not crystal_mesh ) {
        crystal_mesh = std::make_unique<Mesh<ColourVertex>>( crystal_builder.colour( colour ).get_mesh() );
        crystal_mesh->set_flag( IsLightSource );
    }

    glm::vec3 constexpr crystal_scale { 0.1f, 0.3f, 0.1f };
    glm::vec3 constexpr crystal_offset { 0.f, 0.5f, 0.f };
    m_ecs->components.insert_component<Drawable>( crystal, { .mesh = crystal_mesh.get(), .scale = crystal_scale } );
    m_ecs->components.insert_component<Location>( crystal, { .position = position + crystal_offset } );

    return tower;
}
