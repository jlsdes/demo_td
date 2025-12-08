#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"

#include <filesystem>
#include <iostream>


int main()
{
    {
        Window const window { 600, 480, "Demo TD" };

        // Find and build the main graphics shader
        auto const shader_dir { (std::filesystem::path( __FILE__ ) / "../shader").lexically_normal() };
        GraphicsShader const shader { (shader_dir / "main.vert").c_str(), (shader_dir / "main.frag").c_str() };
        shader.use();

        // Create a simple mesh
        float vertices[] {
            -0.5f, -0.5f, 0,
            -0.5f, 0.5f, 0,
            0.5f, 0.5f, 0,
            -0.5f, -0.5f, 0,
            0.5f, 0.5f, 0,
            0.5f, -0.5f, 0
        };
        Mesh const mesh { vertices, 6 };

        // Timekeeping setup
        double interval_start { glfwGetTime() };
        unsigned int frame_counter { 0 };
        double report_interval { 1. };

        // Main program loop
        while ( !window.is_closing() ) {
            glfwPollEvents();
            shader.set_uniform( "time", static_cast<float>(glfwGetTime()) );
            mesh.draw();
            window.render();

            // Timekeeping
            ++frame_counter;
            double const current_time { glfwGetTime() };
            double const elapsed_time { current_time - interval_start };
            if ( elapsed_time >= report_interval ) {
                std::cout << "\rFPS: " << frame_counter / elapsed_time << std::flush;
                frame_counter = 0;
                interval_start = current_time;
            }
        }
        std::cout << std::string( 100, ' ' ) << std::endl;
    }
    glfwTerminate();
    return 0;
}
