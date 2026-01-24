#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "component/drawable.hpp"
#include "system/renderer.hpp"
#include "engine/entity_component_system.hpp"


int main() {
    // auto const main_dir { get_main_dir() };
    // Config::load_config( main_dir / "config.ini" );
    // Log::info( "Loaded config ", (main_dir / "config.ini").string() );
    // glfwTerminate();

    ECS ecs {};
    EntityManager & entities { ecs.entities };
    ComponentManager & components { ecs.components };
    SystemManager & systems { ecs.systems };

    ComponentTypeID const drawable_id { components.create_store<Drawable>() };

    systems.insert_system<Renderer>( id_to_flag( drawable_id ), SystemGroup::Render );

    Entity const entity { entities.create() };
    Log::debug( entity );

    systems.run_group( General );

    return 0;
}
