#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "engine/camera.hpp"
#include "engine/config.hpp"
#include "engine/log.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include "engine/time.hpp"
#include "engine/window.hpp"

#include <filesystem>

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
        DEBUG( "Current average FPS: ", frame_counter/elapsed_time );
        frame_counter = 0;
        interval_start = current_time;
    }
}


int main() {
    auto const main_dir { (std::filesystem::path( __FILE__ ) / "../../").lexically_normal() };
    Config::load_config( main_dir / "config.ini" );
    INFO( "Loaded config", main_dir / "config.ini" );

    // If any glDelete...() function is called after glfwTerminate() has been called, a segfault occurs
    // This code block ensures that all objects go out of scope, and thus have their destructors called with glDelete()
    // calls, before glfwTerminate() at the end of this function
    {
        Window window {};

        // Find and build the main graphics shader
        auto const vertex_shader { main_dir / Config::get<std::string>( "Shader", "vertex_shader" ) };
        auto const fragment_shader { main_dir / Config::get<std::string>( "Shader", "fragment_shader" ) };
        GraphicsShader shader { vertex_shader.c_str(), fragment_shader.c_str() };
        shader.use();

        // Create a circle
        std::vector<Vertex> vertices { 101 };
        std::vector indices { 0u }; // A vector of unsigned ints with a single element 0
        vertices.at( 0 ) = { { 0.f, 0.f, 0.f }, {}, { 0.f, 0.f, 0.f } };
        for ( unsigned int i { 1 }; i < vertices.size(); ++i ) {
            double const angle { (100. - i) / 100. * glm::two_pi<double>() };
            Vertex & vertex { vertices.at( i ) };
            vertex.position = { static_cast<float>(cos( angle )), static_cast<float>(sin( angle )), 0.f };

            // Draw a colour circle
            float const angle_div { static_cast<float>(angle / (glm::pi<double>() / 3)) };
            switch ( static_cast<int>(std::floor( angle_div )) ) {
            case 0: vertex.colour = { 1, (1 - std::abs( angle_div - 1 )), 0 };
                break;
            case 1: vertex.colour = { (1 - std::abs( angle_div - 1 )), 1, 0 };
                break;
            case 2: vertex.colour = { 0, 1, (1 - std::abs( angle_div - 3 )) };
                break;
            case 3: vertex.colour = { 0, (1 - std::abs( angle_div - 3 )), 1 };
                break;
            case 4: vertex.colour = { (1 - std::abs( angle_div - 5 )), 0, 1 };
                break;
            case 5: vertex.colour = { 1, 0, (1 - std::abs( angle_div - 5 )) };
                break;
            default: // Shouldn't happen
                break;
            }
            indices.emplace_back( i );
        }
        indices.emplace_back( 1 );
        Mesh const mesh { vertices, indices, GL_TRIANGLE_FAN };

        // Create a camera object and attach it to the shader
        glm::vec3 const & camera_position { 0.f, 0.f, 3.f };
        glm::vec3 const & camera_target { 0.f, 0.f, 0.f };
        Camera camera { camera_position, camera_target, &shader };
        camera.set_free_view( window.get_input_manager() );

        shader.set_uniform( "model", glm::identity<glm::mat4>() );
        shader.set_uniform( "projection", glm::perspective( glm::radians( 45.f ), 1200.f / 800.f, 0.1f, 100.f ) );

        // Main program loop
        while ( !window.is_closing() ) {
            Time::loop_start();

            glfwPollEvents();
            mesh.draw();
            camera.update();

            window.render();
            // keep_time();
        }
    }
    glfwTerminate();
    return 0;
}
