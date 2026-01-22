#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "engine/component_manager.hpp"
#include "engine/entity_manager.hpp"


struct Position : Component {
    Position() = default;
    Position( float const x, float const y, float const z ) : x { x }, y { y }, z { z } {}

    float x;
    float y;
    float z;
};


int main() {
    // auto const main_dir { get_main_dir() };
    // Config::load_config( main_dir / "config.ini" );
    // Log::info( "Loaded config ", (main_dir / "config.ini").string() );
    // glfwTerminate();

    return 0;
}
