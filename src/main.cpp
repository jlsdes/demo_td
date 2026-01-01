#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "engine/camera.hpp"
#include "engine/config.hpp"
#include "engine/log.hpp"
#include "engine/mesh.hpp"
#include "engine/render_object.hpp"
#include "engine/renderer.hpp"
#include "engine/shader.hpp"
#include "engine/mesh_builder.hpp"
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
        Log::debug( "Current average FPS: ", frame_counter / elapsed_time );
        frame_counter = 0;
        interval_start = current_time;
    }
}


int main() {
    auto const main_dir { (std::filesystem::path( __FILE__ ) / "../../").lexically_normal() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

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

        glm::vec3 constexpr ambient_light { 0.1f, 0.1f, 0.1f };
        shader.set_uniform( "ambient_light", ambient_light );
        shader.set_uniform( "sun_light", glm::vec3 { 1.f, 1.f, 1.f } );
        glm::vec3 constexpr sun_direction { -0.2f, 1.f, -0.5f };
        shader.set_uniform( "sun_direction", sun_direction );

        // Draw triangles as wireframes when F is being pressed
        auto & input_manager { window.get_input_manager() };
        input_manager.observe_keyboard( GLFW_KEY_F, [&]( int const, int const action ) {
            if ( action == GLFW_PRESS ) {
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                shader.set_uniform( "ambient_light", glm::vec3 { 1.f, 1.f, 1.f } );
                // glDisable( GL_CULL_FACE );
            } else if ( action == GLFW_RELEASE ) {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                shader.set_uniform( "ambient_light", ambient_light );
                // glEnable( GL_CULL_FACE );
            }
        } );

        // Create a camera object and attach it to the shader
        glm::vec3 const & camera_position { 0.f, 0.f, -3.f };
        glm::vec3 const & camera_target { 0.f, 0.f, 0.f };
        Camera camera { camera_position, camera_target, &shader };
        camera.set_free_view( window.get_input_manager() );

        Renderer renderer {};
        std::vector<std::unique_ptr<RenderObject>> render_objects {};
        MeshBuilder builder { MeshBuilder::sphere( 10 ) };
        for ( unsigned char i { 0 }; i < 1; ++i ) {
            glm::vec3 offset { i & 4 ? -0.5f : 0.5f, i & 2 ? -0.5f : 0.5f, i & 1 ? -0.5f : 0.5f };
            builder.m_colours = { builder.m_vertices.size(), offset + glm::vec3 { 0.5f } };
            render_objects.push_back(
                std::make_unique<RenderObject>( RenderObject::Opaque, builder.get_mesh(), &shader ) );
            RenderObject & object { *render_objects.back() };
            object.translate( camera_target + offset );
            object.scale( 0.3 );
            renderer.register_object( object );
        }

        float constexpr fov { glm::quarter_pi<float>() }; // 45 degrees
        shader.set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );

        // Main program loop
        while ( !window.is_closing() ) {
            Time::loop_start();
            window.clear();

            glfwPollEvents();
            renderer.draw();
            camera.update();
            window.render();
            // keep_time();
        }
    }
    glfwTerminate();
    return 0;
}
