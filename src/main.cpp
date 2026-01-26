#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"
#include "engine/camera.hpp"
#include "engine/mesh_builder.hpp"
#include "engine/shader.hpp"
#include "engine/window.hpp"
#include "engine/context.hpp"

#include "component/drawable.hpp"
#include "component/position.hpp"
#include "system/renderer.hpp"
#include "engine/entity_component_system.hpp"


struct Params {
    std::unique_ptr<Window> window { nullptr };
    std::unique_ptr<GraphicsShader> shader { nullptr };
    std::unique_ptr<Camera> camera { nullptr };
};

Params initialise() {
    Params params {};

    if ( not glfwInit() ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLFW" );
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 ); // Enable MSAA (anti-aliasing)

    params.window = std::make_unique<Window>();

    if ( not gladLoadGL( glfwGetProcAddress ) ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLAD." );
    }
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );

    // Find and build the main graphics shader
    auto const vertex_shader { get_main_dir() / Config::get<std::string>( "Shader", "vertex_shader" ) };
    auto const fragment_shader { get_main_dir() / Config::get<std::string>( "Shader", "fragment_shader" ) };
    params.shader = std::make_unique<GraphicsShader>( vertex_shader.c_str(), fragment_shader.c_str() );
    params.shader->use();

    glm::vec3 constexpr ambient_light { 0.01f };
    params.shader->set_uniform( "ambient_light", ambient_light );
    params.shader->set_uniform( "sun_light", glm::vec3 { 1.f, 1.f, 1.f } );
    glm::vec3 constexpr sun_direction { -0.2f, 1.f, -0.5f };
    params.shader->set_uniform( "sun_direction", sun_direction );

    float constexpr fov { std::numbers::pi_v<float> / 4.f }; // 45 degrees
    params.shader->set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );

    glm::vec3 constexpr position { -3.f, 0.f, 0.f };
    glm::vec3 constexpr target { 0.f, 0.f, 0.f };
    params.camera = std::make_unique<Camera>( position, target, params.shader.get() );

    params.camera->set_free_view( params.window->get_input_manager() );

    return params;
}


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    auto [window, shader, camera] { initialise() };

    ECS ecs {};
    EntityManager & entities { ecs.entities };
    ComponentManager & components { ecs.components };
    SystemManager & systems { ecs.systems };

    systems.insert_system<Renderer>( SystemGroup::Render );

    EntityID const entity { entities.create() };

    // Create a sphere
    glm::vec3 constexpr red { 1.f, 0.f, 0.f };
    auto builder { MeshBuilder::sphere( 10 ).colour( red ) };
    auto mesh { builder.get_mesh() };

    Drawable sphere {};
    sphere.mesh = &mesh;
    sphere.shader = shader.get();
    // sphere.position = glm::vec3 { 0.f, 0.5f, 0.f };
    // sphere.rotation = glm::quatLookAt( glm::normalize( glm::vec3 { 0.5f, 0.5f, 0.f } ),
    //                                        glm::vec3 { 0.f, 1.f, 0.f } );
    sphere.scale = glm::vec3 { 0.5f };
    components.insert_component( entity, sphere );

    Position position { .position = { 0.f, 0.f, 0.f } };
    components.insert_component( entity, position );

    while ( not window->is_closing() ) {
        Time::loop_start();
        window->clear();

        glfwPollEvents();
        camera->update();
        systems.run_group( Render );

        window->render();
    }

    return 0;
}
