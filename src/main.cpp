#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"
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

    return params;
}


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    auto [window] { initialise() };

    ECS ecs {};
    EntityManager & entities { ecs.entities };
    ComponentManager & components { ecs.components };
    SystemManager & systems { ecs.systems };

    components.create_store<Drawable>();
    components.create_store<Position>();

    auto renderer { std::make_unique<Renderer>( *window ) };

    EntityID const entity { entities.create() };

    // Create a sphere
    glm::vec3 constexpr red { 1.f, 0.f, 0.f };
    auto builder { MeshBuilder::sphere( 10 ).colour( red ) };
    auto mesh { builder.get_mesh() };

    Drawable sphere {};
    sphere.mesh = &mesh;
    sphere.shader = &renderer->get_shader();
    // sphere.position = glm::vec3 { 0.f, 0.5f, 0.f };
    // sphere.rotation = glm::quatLookAt( glm::normalize( glm::vec3 { 0.5f, 0.5f, 0.f } ),
    //                                        glm::vec3 { 0.f, 1.f, 0.f } );
    sphere.scale = glm::vec3 { 0.5f };
    components.insert_component( entity, sphere );

    Position position { .position = { 0.f, 0.f, 0.f } };
    components.insert_component( entity, position );

    systems.insert_system<Renderer>( std::move( renderer ), Render );

    while ( not window->is_closing() ) {
        Time::loop_start();
        window->clear();

        glfwPollEvents();
        // camera->update();
        systems.run_group( Render );

        window->render();
    }

    return 0;
}
