#include "renderer.hpp"
#include "component/component_manager.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "utils/config.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <deque>
#include <queue>
#include <ranges>
#include <set>
#include <sstream>


glm::vec3 constexpr g_initial_position { -3.f, 0.f, 0.f };
glm::vec3 constexpr g_initial_target { 0.f, 0.f, 0.f };


Renderer::Renderer( ECS * const ecs, Window & window )
    : System { ecs }, m_window { window },
      m_camera { std::make_unique<Camera>( g_initial_position, g_initial_target ) }, m_shaders {} {

    m_camera->set_free_view( m_window.get_input_manager() );

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
        shader.set_uniform( "is_light_source", false );
    }
}

glm::mat4 compute_transformation( Drawable const & drawable, Location const * const position ) {
    auto transformation { glm::identity<glm::mat4>() };
    if ( position )
        transformation = glm::translate( transformation, position->position );
    transformation = glm::scale( transformation, drawable.scale );
    transformation *= glm::mat4_cast( drawable.orientation );
    return transformation;
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
    m_camera->update();
    for ( auto const & shader : std::views::values( m_shaders ) )
        m_camera->update_shader( shader );

    ComponentFlags const position_flag { id_to_flag( m_ecs->components.get_type_id<Location>() ) };

    // Each instanced mesh needs to be drawn only once, so keeping track of the ones seen avoids duplicates in the queue
    std::set<InstancedMesh<ColourVertex> const *> instanced_meshes {};

    // Push all Drawable components into a priority queue
    RenderQueue render_queue {};
    for ( auto iterator { components.begin<Drawable>() }; iterator != components.end<Drawable>(); ++iterator ) {
        EntityID const entity { iterator.get_entity() };
        Drawable & drawable { iterator.get_component() };

        Location const * position { nullptr };
        if ( m_ecs->entities.has_flags( entity, position_flag ) )
            position = &components.get_component<Location>( entity );

        if ( drawable.mesh->get_flag( IsInstanced ) ) {
            auto const mesh { dynamic_cast<InstancedMesh<ColourVertex> *>(drawable.mesh) };
            bool const first_instance { not instanced_meshes.contains( mesh ) };

            // The list of instances should be cleared between every render loop, and then each instance is added again
            if ( first_instance )
                mesh->reset_data();
            mesh->update( compute_transformation( drawable, position ) );

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

            glm::mat4 const transformation { compute_transformation( *drawable, position ) };
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
}

Shader & Renderer::get_shader( std::string const & name ) {
    return m_shaders.get_shader( name );
}
