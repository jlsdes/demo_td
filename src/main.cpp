#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"

#include <filesystem>
#include <iostream>


/** Reports the framerate of the program at regular intervals; to be called after rendering each frame. */
void keep_time()
{
    // Time between each print in seconds
    double constexpr report_interval { 1. };

    // The function's memory
    static double interval_start { glfwGetTime() };
    static unsigned int frame_counter { 0 };

    ++frame_counter;
    double const current_time { glfwGetTime() };
    double const elapsed_time { current_time - interval_start };
    if ( elapsed_time >= report_interval ) {
        std::cout << "\rFPS: " << frame_counter / elapsed_time << std::flush;
        frame_counter = 0;
        interval_start = current_time;
    }
}


int main()
{
    {
        Window const window { 600, 480, "Demo TD" };

        // Find and build the main graphics shader
        auto const shader_dir { Shader::get_shader_directory() };
        GraphicsShader const shader { (shader_dir / "main.vert").c_str(), (shader_dir / "main.frag").c_str() };
        shader.use();

        // Create a simple mesh
        std::vector<float> const vertices {
            -0.5, 0.5, 0,
            0.5, 0.5, 0,
            -0.5, -0.5, 0,
            0.5, -0.5, 0
        };
        std::vector<unsigned int> const indices {
            0, 1, 2, 3
        };
        Mesh const mesh { vertices, GL_TRIANGLE_STRIP };

        // Main program loop
        while ( !window.is_closing() ) {
            glfwPollEvents();
            shader.set_uniform( "time", static_cast<float>(glfwGetTime()) );
            mesh.draw();
            window.render();

            keep_time();
        }
        std::cout << std::string( 100, ' ' ) << std::endl;
    }
    glfwTerminate();
    return 0;
}
