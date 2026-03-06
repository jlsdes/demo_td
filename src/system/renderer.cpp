#include "renderer.hpp"

#include "component/component_manager.hpp"
#include "component/drawable.hpp"
#include "component/entity_type.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "component/tower_data.hpp"

#include "core/mesh.hpp"
#include "core/mesh_builder.hpp"
#include "core/shader.hpp"

#include "utils/config.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <deque>
#include <queue>
#include <ranges>
#include <set>
#include <sstream>


Renderer::Renderer( ECS * const ecs, Window & window, Camera & camera )
    : System { ecs }, m_window { window }, m_camera { camera }, m_shaders {} {
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
}

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
    return InstancedMesh<ColourVertex> { builder.get_mesh() };
}

static void render_tile( EntityID const entity, ECS * const ecs, ShaderStore & shader_store ) {
    static InstancedMesh<ColourVertex> mesh { build_tile_mesh() };

    Shader const & shader { shader_store.get_shader( "instanced" ) };
    shader.set_uniform( "nr_instances", mesh.get_nr_instances() );

    if ( not mesh.get_flag( IsInitialised ) )
        mesh.initialise_gl_objects();
    mesh.draw();
}

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
        *(iter++) = std::make_unique<Mesh<ColourVertex>>( base.colour( TowerData::colours[i] ).get_mesh() );
        *(iter++) = std::make_unique<Mesh<ColourVertex>>( crystal.colour( TowerData::colours[i] ).get_mesh() );
    }
    return meshes;
}

/// Renders a single tower.
static void render_tower( EntityID const entity, ECS * const ecs, ShaderStore & shader_store ) {
    static auto const meshes { build_tower_meshes() };

    Location const & location { ecs->components.get_component<Location>( entity ) };
    glm::mat4 const transformation { glm::translate( glm::identity<glm::mat4>(), location.position ) };

    auto const & shader { shader_store.get_shader( "main" ) };
    shader.set_uniform( "model", transformation );
    shader.set_uniform( "normal_transform", glm::mat3 { glm::transpose( glm::inverse( transformation ) ) } );

    TowerData const & tower { ecs->components.get_component<TowerData>( entity ) };
    auto const & base_mesh { meshes.at( tower.type * 2 ) };
    auto const & crystal_mesh { meshes.at( tower.type * 2 + 1 ) };

    if ( not base_mesh->get_flag( IsInitialised ) ) {
        base_mesh->initialise_gl_objects();
        crystal_mesh->initialise_gl_objects();
    }

    base_mesh->draw();
    shader.set_uniform( "is_light_source", true );
    crystal_mesh->draw();
    shader.set_uniform( "is_light_source", false );
}

/// Renders a single entity.
static void render_entity( EntityID const entity, EntityType const type, ECS * const ecs, ShaderStore & shader_store ) {
    switch ( auto const type_id { type.type_id } ) {
    case EntityType::Tile:

        break;
    case EntityType::Tower:
        render_tower( entity, ecs, shader_store );
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

struct QueueItem {
    Drawable * drawable;
    Location const * position;
    float priority;
};

struct CompareItems {
    bool constexpr operator()( QueueItem const & lhs, QueueItem const & rhs ) const {
        return lhs.priority < rhs.priority;
    }
};

using RenderQueue = std::priority_queue<QueueItem, std::deque<QueueItem>, CompareItems>;

void Renderer::run() {
    auto & components { m_ecs->components };

    // Update the camera attributes, and make sure all shaders are synchronised on this
    m_camera.update();
    for ( auto const & shader : std::views::values( m_shaders ) )
        m_camera.update_shader( shader );

    ComponentFlags const position_flag { id_to_flag( m_ecs->components.get_type_id<Location>() ) };

    // Each instanced mesh needs to be drawn only once, so keeping track of the ones seen avoids duplicates in the queue
    std::set<InstancedMesh<ColourVertex> const *> instanced_meshes {};

    // Push all Drawable components into a priority queue
    RenderQueue render_queue {};
    for ( auto iterator { components.begin<Drawable>() }; iterator != components.end<Drawable>(); ++iterator ) {
        EntityID const entity { iterator.get_entity() };
        Drawable & drawable { iterator.get_component() };

        if ( drawable.mesh->get_flag( IsHidden ) )
            continue;

        Location const * position { nullptr };
        if ( m_ecs->entities.has_flags( entity, position_flag ) )
            position = &components.get_component<Location>( entity );

        if ( drawable.mesh->get_flag( IsInstanced ) ) {
            auto const mesh { dynamic_cast<InstancedMesh<ColourVertex> *>(drawable.mesh) };
            bool const first_instance { not instanced_meshes.contains( mesh ) };

            // For 'dynamic' instanced meshes, clear the instances each loop and add them again
            if ( mesh->get_flag( IsDynamic ) ) {
                if ( first_instance )
                    mesh->clear_instances();
                mesh->add_instance( drawable.scale, drawable.orientation, position ? position->position : origin );
            }

            // If this is the first instance, add the mesh to the render queue, and add it to the set of seen meshes
            if ( not first_instance )
                continue;
            instanced_meshes.emplace( mesh );
        }
        render_queue.push( { &drawable, position, static_cast<float>(drawable.priority) } );
    }

    Shader const * shader;

    while ( not render_queue.empty() ) {
        auto const [drawable, position, _] = render_queue.top();
        render_queue.pop();

        auto const & mesh { drawable->mesh };

        if ( not mesh->get_flag( IsInstanced ) ) {
            shader = &m_shaders.get_shader( "main" );

            glm::mat4 const transformation {
                compute_transformation( drawable->scale, drawable->orientation, position ? position->position : origin )
            };
            shader->set_uniform( "model", transformation );
            shader->set_uniform( "normal_transform", glm::mat3 { glm::transpose( glm::inverse( transformation ) ) } );
        } else {
            shader = &m_shaders.get_shader( "instanced" );

            auto const instanced_mesh { dynamic_cast<InstancedMesh<ColourVertex> *>(mesh) };
            shader->set_uniform( "nr_instances", instanced_mesh->get_nr_instances() );
        }

        bool const light_source { mesh->get_flag( IsLightSource ) };
        if ( light_source )
            shader->set_uniform( "is_light_source", true );

        if ( not mesh->get_flag( IsInitialised ) )
            mesh->initialise_gl_objects();
        mesh->draw();

        if ( light_source )
            shader->set_uniform( "is_light_source", false );
    }

    for ( auto iterator { components.begin<EntityType>() }; iterator != components.end<EntityType>(); ++iterator ) {
        render_entity( iterator.get_entity(), iterator.get_component(), m_ecs, m_shaders );
    }
}
