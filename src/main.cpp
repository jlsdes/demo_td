#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "engine/engine.hpp"
#include "utils/config.hpp"
#include "utils/log.hpp"


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    Engine engine {};
    engine.run();

    glfwTerminate();
    return 0;
}
