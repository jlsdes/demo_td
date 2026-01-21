#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    glfwTerminate();
    return 0;
}

// TODO fix window resizing && the F key callback function not working
