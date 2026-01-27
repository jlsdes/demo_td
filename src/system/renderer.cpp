#include "renderer.hpp"
#include "component/component_manager.hpp"
#include "component/drawable.hpp"
#include "component/position.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include "utils/config.hpp"

#include <glm/gtc/matrix_transform.hpp>


glm::vec3 constexpr g_initial_position { -3.f, 0.f, 0.f };
glm::vec3 constexpr g_initial_target { 0.f, 0.f, 0.f };


Renderer::Renderer( Window & window ) : m_window { window },
                                        m_camera { std::make_unique<Camera>( g_initial_position, g_initial_target ) },
                                        m_shaders {} {
    m_camera->set_free_view( m_window.get_input_manager() );

    // Find and build the main graphics shader
    auto const vertex_shader { Config::get<std::filesystem::path>( "Shader", "vertex_shader" ) };
    auto const fragment_shader { Config::get<std::filesystem::path>( "Shader", "fragment_shader" ) };

    auto [shader_id, shader] { m_shaders.emplace_shader( vertex_shader, fragment_shader ) };
    shader.use();

    glm::vec3 constexpr ambient_light { 0.01f };
    shader.set_uniform( "ambient_light", ambient_light );
    shader.set_uniform( "sun_light", glm::vec3 { 1.f, 1.f, 1.f } );
    glm::vec3 constexpr sun_direction { -0.2f, 1.f, -0.5f };
    shader.set_uniform( "sun_direction", sun_direction );
    float constexpr fov { std::numbers::pi_v<float> / 4.f }; // 45 degrees
    shader.set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );
}

glm::mat4 compute_transformation( Drawable const & drawable, Position const * const position ) {
    auto transformation { glm::identity<glm::mat4>() };
    if ( position )
        transformation = glm::translate( transformation, position->position );
    transformation = glm::scale( transformation, drawable.scale );
    transformation *= glm::mat4_cast( drawable.rotation );
    return transformation;
}

void Renderer::run( EntityManager const & entities, ComponentManager & components ) {
    m_camera->update();
    for ( auto const & shader : m_shaders )
        m_camera->update_shader( shader );

    // For now, everything uses the same shader
    Shader const & shader { m_shaders.get_shader( 0 ) };
    shader.use();

    ComponentFlags const position_flag { id_to_flag( components.get_type_id<Position>() ) };

    for ( auto iterator { components.begin<Drawable>() }; iterator != components.end<Drawable>(); ++iterator ) {
        EntityID const entity { iterator.get_entity() };
        Drawable & drawable { iterator.get_component() };

        Position const * position { nullptr };
        if ( entities.has_flags( entity, position_flag ) )
            position = &components.get_component<Position>( entity );

        shader.set_uniform( "model",  compute_transformation( drawable, position )  );

        if ( not drawable.mesh->is_initialised() )
            drawable.mesh->initialise_gl_objects();
        drawable.mesh->draw();
    }
}

Shader & Renderer::get_shader( unsigned int const shader_id ) {
    return m_shaders.get_shader( shader_id );
}
