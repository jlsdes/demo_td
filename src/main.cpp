#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "engine/component_manager.hpp"
#include "engine/entity_manager.hpp"
#include "engine/system_manager.hpp"


struct Position : Component {
    Position() = default;
    Position( float const x, float const y, float const z ) : x { x }, y { y }, z { z } {}

    float x;
    float y;
    float z;
};


class MovingSystem : public System {
public:
    MovingSystem( ComponentFlag const flags, ComponentManager * const component_manager )
        : System { flags, component_manager } {}

    void run() override {
        Log::debug( "RUNNING" );
    }
};


int main() {
    // auto const main_dir { get_main_dir() };
    // Config::load_config( main_dir / "config.ini" );
    // Log::info( "Loaded config ", (main_dir / "config.ini").string() );
    // glfwTerminate();

    ComponentManager component_manager {};
    component_manager.create_store<Position>();

    SystemManager system_manager { &component_manager };
    system_manager.insert_system<MovingSystem>( SystemGroup::General );

    EntityManager entity_manager {};
    for ( unsigned int i { 0 }; i < g_max_entities; ++i ) {
        entity_manager.create( i );
    }

    return 0;
}
