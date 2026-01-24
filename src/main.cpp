#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "engine/entity_component_system.hpp"


struct Position : Component {
    Position() = default;
    Position( float const x, float const y, float const z ) : x { x }, y { y }, z { z } {}

    float x;
    float y;
    float z;
};


class MovingSystem : public System {
public:
    explicit MovingSystem( ComponentFlags const flags ) : System { flags } {}

    void run( EntityManager const & entities, ComponentManager & components ) override {
        Log::debug( "RUNNING" );
    }
};


int main() {
    // auto const main_dir { get_main_dir() };
    // Config::load_config( main_dir / "config.ini" );
    // Log::info( "Loaded config ", (main_dir / "config.ini").string() );
    // glfwTerminate();

    ECS ecs {};
    EntityManager & entities { ecs.entities };
    ComponentManager & components { ecs.components };
    SystemManager & systems { ecs.systems };

    ComponentTypeID const position_id { components.create_store<Position>() };
    systems.insert_system<MovingSystem>( id_to_flag( position_id ) );

    Entity const entity { entities.create() };
    Log::debug( entity );
    components.insert_component( entity, Position {} );

    systems.run_group( General );

    return 0;
}
