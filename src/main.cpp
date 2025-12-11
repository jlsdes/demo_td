#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "engine/camera.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include "engine/time.hpp"
#include "engine/window.hpp"

#include <filesystem>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>


/** Reports the framerate of the program at regular intervals; to be called after rendering each frame. */
void keep_time() {
    // Time between each print in seconds
    double constexpr report_interval { 1. };

    // The function's memory
    static double interval_start { Time::get_time() };
    static unsigned int frame_counter { 0 };

    ++frame_counter;
    double const current_time { Time::get_time() };
    double const elapsed_time { current_time - interval_start };
    if ( elapsed_time >= report_interval ) {
        std::cout << "\rFPS: " << frame_counter / elapsed_time << std::flush;
        frame_counter = 0;
        interval_start = current_time;
    }
}


int main() {
    // If any glDelete...() function is called after glfwTerminate() has been called, a segfault occurs
    // This code block ensures that all objects go out of scope, and thus have their destructors called with glDelete()
    // calls, before glfwTerminate() at the end of this function
    {
        Window window { 600, 480, "Demo TD" };

        // Find and build the main graphics shader
        auto const shader_dir { Shader::get_shader_directory() };
        GraphicsShader shader { (shader_dir / "main.vert").c_str(), (shader_dir / "main.frag").c_str() };
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

        // Create a camera object and attach it to the shader
        glm::vec3 const & camera_position { 0.f, 0.f, 3.f };
        glm::vec3 const & camera_target { 0.f, 0.f, 0.f };
        Camera camera { camera_position, camera_target, &shader };
        camera.set_free_view( window.get_input_manager() );

        shader.set_uniform( "model", glm::identity<glm::mat4>() );
        shader.set_uniform( "projection", glm::perspective( glm::radians( 45.f ), 600.f / 400.f, 0.1f, 100.f ) );

        // Main program loop
        while ( !window.is_closing() ) {
            Time::loop_start();

            glfwPollEvents();
            mesh.draw();
            camera.update();

            window.render();
            keep_time();
        }
        // The keep_time function has been overwriting the same line, finally go to the next line
        std::cout << std::endl;
    }
    glfwTerminate();
    return 0;
}
