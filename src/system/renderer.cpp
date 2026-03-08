#include "renderer.hpp"

#include "tile_manager.hpp"

#include "component/component_manager.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "component/tower_data.hpp"

#include "core/mesh.hpp"
#include "core/mesh_builder.hpp"
#include "core/shader.hpp"

#include "utils/config.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <deque>
#include <numbers>
#include <ranges>
#include <sstream>


/// Generates the mesh used to draw the tiles.
static InstancedMesh<ColourVertex> build_tile_mesh() {
    glm::vec3 constexpr y_offset { 0.f, 0.001f, 0.f };
    float constexpr main_weight { 0.95f };
    float constexpr centre_weight { 1.f - main_weight };

    std::vector<glm::vec3> vertices { tile_position( 0, 0 ), tile_position( 0, 1 ), tile_position( 1, 1 ) };
    glm::vec3 const centre { (vertices[0] + vertices[1] + vertices[2]) / 3.f };
    vertices.emplace_back( main_weight * vertices[0] + centre_weight * centre + y_offset );
    vertices.emplace_back( main_weight * vertices[1] + centre_weight * centre + y_offset );
    vertices.emplace_back( main_weight * vertices[2] + centre_weight * centre + y_offset );

    std::vector<std::vector<unsigned int>> const faces { { 0, 1, 2 }, { 3, 4, 5 } };

    glm::vec3 constexpr main_colour { 0.5f, 0.5f, 0.5f };
    glm::vec3 constexpr border_colour { 0.2f, 0.2f, 0.2f };
    std::vector const colours { border_colour, border_colour, border_colour, main_colour, main_colour, main_colour };

    MeshBuilder builder { vertices, faces, {}, colours };
    builder.generate_face_normals();

    InstancedMesh<ColourVertex> result { builder.get_mesh() };
    result.initialise_gl_objects();
    return result;
}

/// Helper class to draw specifically tiles.
class TileRenderer : public SubRenderer {
public:
    explicit TileRenderer( Renderer * const renderer ) : SubRenderer { renderer }, m_mesh { build_tile_mesh() } {}

    /// Updates the tile mesh data if necessary.
    void update( EntityID const entity ) override {
        if ( not m_updating_tiles )
            return;

        if ( m_first_tile ) {
            m_first_tile = false;
            m_mesh.clear_instances();
        }

        static std::array<glm::mat3, 2> const orientations {
            glm::mat3_cast( glm::quat {} ),
            glm::mat3_cast( glm::quat { glm::vec3 { 0.f, std::numbers::pi_v<float> / 3.f, 0.f } } )
        };

        auto const & tile { m_parent->m_ecs->components.get_component<TerrainTile>( entity ) };
        m_mesh.add_instance( glm::vec3 { 1.f }, orientations[tile.tile_id.half], tile_position( tile.tile_id ) );
    }

    /// Draws all tiles at once.
    void finish() override {
        auto const & shader { m_parent->m_shaders.get_shader( "instanced" ) };
        shader.set_uniform( "nr_instances", m_mesh.get_nr_instances() );
        m_mesh.draw();
    }

    /// To be called before every render loop. Sets all flags to their correct initial value.
    void start() override {
        m_first_tile = true;
        m_updating_tiles = m_parent->m_ecs->systems.get_system<TileManager>()->has_updated();
    }

private:
    InstancedMesh<ColourVertex> m_mesh;

    bool m_first_tile { true };
    bool m_updating_tiles { false };
};

Renderer::Renderer( ECS * const ecs, Window & window, Camera & camera )
    : System { ecs }, m_window { window }, m_camera { camera }, m_shaders {}, m_sub_renderers { nullptr } {
    glm::vec3 constexpr ambient_light { 0.01f };
    glm::vec3 constexpr sun_light { 1.f };
    glm::vec3 constexpr sun_direction { -0.2f, 1.f, -0.5f };
    float constexpr fov { std::numbers::pi_v<float> / 4.f }; // 45 degrees

    // The config file should contain a list of shader programs in Shader::shaders, separated by spaces. Each shader
    // must also have (at least) two additional fields <name>_vert and <name>_frag.
    std::stringstream stream { Config::get<std::string>( "Shader", "shaders" ) };
    std::string name;
    while ( stream >> name ) {
        auto const vertex_shader { Config::get<std::filesystem::path>( "Shader", name + "_vert" ) };
        auto const fragment_shader { Config::get<std::filesystem::path>( "Shader", name + "_frag" ) };
        auto const & shader { m_shaders.emplace_shader( name, vertex_shader, fragment_shader ) };

        shader.use();
        shader.set_uniform( "ambient_light", ambient_light );
        shader.set_uniform( "sun_light", sun_light );
        shader.set_uniform( "sun_direction", sun_direction );
        shader.set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );
        shader.set_uniform( "nr_lights", 0 );
        shader.set_uniform( "is_light_source", false );
    }

    m_sub_renderers[EntityType::Tile] = std::make_unique<TileRenderer>( this );
}

Renderer::~Renderer() = default;

/// Returns an array of meshes, where each tower type can find its meshes at indices 2*type and 2*type+1.
static std::array<std::unique_ptr<Mesh<ColourVertex>>, TowerData::NumberTypes * 2> constexpr build_tower_meshes() {
    Log::info( "Generating tower meshes." );

    static glm::vec3 constexpr base_scale { 0.3f, 0.1f, 0.3f };
    static glm::vec3 constexpr crystal_scale { 0.1f, 0.3f, 0.1f };
    static glm::vec3 constexpr crystal_offset { 0.f, 0.5f, 0.f };

    MeshBuilder base { MeshBuilder::cube() };
    base.transform( glm::scale( glm::identity<glm::mat4>(), base_scale ) );

    MeshBuilder crystal { MeshBuilder::octahedron() };
    crystal.transform( glm::scale( glm::identity<glm::mat4>(), crystal_scale ) );
    crystal.translate( crystal_offset );

    std::array<std::unique_ptr<Mesh<ColourVertex>>, TowerData::NumberTypes * 2> meshes { nullptr };
    auto iter { meshes.begin() };
    for ( unsigned int i { 0 }; i < TowerData::NumberTypes; ++i ) {
        iter[0] = std::make_unique<Mesh<ColourVertex>>( base.colour( TowerData::colours[i] ).get_mesh() );
        iter[0]->initialise_gl_objects();
        iter[1] = std::make_unique<Mesh<ColourVertex>>( crystal.colour( TowerData::colours[i] ).get_mesh() );
        iter[1]->initialise_gl_objects();
        iter += 2;
    }
    return meshes;
}

/// Renders a single tower.
void Renderer::render_tower( EntityID const entity ) {
    static auto const meshes { build_tower_meshes() };

    Location const & location { m_ecs->components.get_component<Location>( entity ) };
    glm::mat4 const transformation { glm::translate( glm::identity<glm::mat4>(), location.position ) };

    auto const & shader { m_shaders.get_shader( "main" ) };
    shader.set_uniform( "model", transformation );
    shader.set_uniform( "normal_transform", glm::mat3 { glm::transpose( glm::inverse( transformation ) ) } );

    TowerData const & tower { m_ecs->components.get_component<TowerData>( entity ) };
    auto const & base_mesh { meshes.at( tower.type * 2 ) };
    auto const & crystal_mesh { meshes.at( tower.type * 2 + 1 ) };

    base_mesh->draw();
    shader.set_uniform( "is_light_source", true );
    crystal_mesh->draw();
    shader.set_uniform( "is_light_source", false );
}

void Renderer::run() {
    auto & components { m_ecs->components };

    // Update the camera attributes, and make sure all shaders are synchronised on this
    m_camera.update();
    for ( auto const & shader : std::views::values( m_shaders ) )
        m_camera.update_shader( shader );

    for ( unsigned int i { 0 }; i < EntityType::NrTypes; ++i ) {
        if ( m_sub_renderers[i] )
            m_sub_renderers[i]->start();
    }

    // TODO reintroduce the render queue
    for ( auto iterator { components.begin<EntityType>() }; iterator != components.end<EntityType>(); ++iterator ) {
        auto const & entity { iterator.get_entity() };
        auto const & type { iterator.get_component() };

        switch ( auto const type_id { type.type_id } ) {
        case EntityType::Tile:
            m_sub_renderers[type_id]->update( entity );
            break;
        case EntityType::Tower:
            render_tower( entity );
            break;
        case EntityType::Enemy:
        case EntityType::Projectile:
        case EntityType::Ui:
        case EntityType::Skybox:
        case EntityType::Other:
        default:
            // Report unrecognised entity types, but only once
            static bool unrecognised_types[256] { false };
            if ( not unrecognised_types[type_id] ) {
                Log::warning( "Entity type ", type_id, " is not recognised by the renderer." );
                unrecognised_types[type_id] = true;
            }
        }
    }

    for ( unsigned int i { 0 }; i < EntityType::NrTypes; ++i ) {
        if ( m_sub_renderers[i] )
            m_sub_renderers[i]->finish();
    }
}
