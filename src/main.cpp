#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "component/component_manager.hpp"
#include "entity/entity_manager.hpp"
#include "system/system_manager.hpp"


struct Position : Component {
    Position() = default;
    Position( float const x, float const y, float const z ) : x { x }, y { y }, z { z } {}

    float x;
    float y;
    float z;
};


class MovingSystem : public System {
public:
    MovingSystem( ComponentFlags const flags, ComponentManager * const component_manager )
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

    return 0;
}
