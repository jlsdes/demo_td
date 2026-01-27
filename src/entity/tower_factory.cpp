#include "tower_factory.hpp"
#include "engine/entity_component_system.hpp"
#include "engine/mesh_builder.hpp"
#include "component/drawable.hpp"
#include "component/position.hpp"


std::array<std::unique_ptr<Mesh<ColourVertex>>, TowerData::NumberTypes> TowerFactory::s_meshes;

TowerFactory::TowerFactory( ECS * const ecs ) : m_ecs { ecs } {
    static bool initialised { false };
    if ( initialised )
        return;

    MeshBuilder builder { MeshBuilder::cube() };
    for ( unsigned char type { 0 }; type < TowerData::NumberTypes; ++type ) {
        builder.colour( s_colours.at( type ) );
        s_meshes.at( type ) = std::make_unique<Mesh<ColourVertex>>( builder.get_mesh() );
    }
    initialised = true;
}

EntityID TowerFactory::build( TowerData::Type const type, glm::vec3 const & position ) const {
    EntityID const entity { m_ecs->entities.create() };

    m_ecs->components.insert_component<TowerData>( entity, { .type = type } );
    m_ecs->components.insert_component<Drawable>( entity, {
                                                      .mesh = s_meshes.at( type ).get(),
                                                      .scale = glm::vec3 { 0.2f }
                                                  } );
    m_ecs->components.insert_component<Position>( entity, { .position = position } );

    return entity;
}
