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


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    TopContext context {};
    ECS & ecs { *context.get_ecs() };

    EntityID const entity { ecs.entities.create() };

    // Create a sphere
    glm::vec3 constexpr red { 1.f, 0.f, 0.f };
    auto builder { MeshBuilder::sphere( 10 ).colour( red ) };
    auto mesh { builder.get_mesh() };

    Drawable sphere {};
    sphere.mesh = &mesh;
    // sphere.position = glm::vec3 { 0.f, 0.5f, 0.f };
    // sphere.rotation = glm::quatLookAt( glm::normalize( glm::vec3 { 0.5f, 0.5f, 0.f } ),
    //                                        glm::vec3 { 0.f, 1.f, 0.f } );
    sphere.scale = glm::vec3 { 0.5f };
    ecs.components.insert_component( entity, sphere );

    Position position { .position = { 0.f, 0.f, 0.f } };
    ecs.components.insert_component( entity, position );

    auto window { context.get_window() };
    while ( not window->is_closing() ) {
        Time::loop_start();
        window->clear();

        glfwPollEvents();
        ecs.systems.run_group( Render );

        window->render();
    }

    return 0;
}
