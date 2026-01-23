#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "engine/coordinator.hpp"


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

    Coordinator coordinator {};

    ComponentFlag const flag { coordinator.insert_component_type<Position>() };
    coordinator.insert_system<MovingSystem>( flag );

    Entity entity { coordinator.insert_entity() };
    Log::debug(entity);

    return 0;
}
