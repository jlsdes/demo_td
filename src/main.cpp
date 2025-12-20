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
    Log::info( "Loaded config", main_dir / "config.ini" );

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

        Renderer renderer {};
        std::vector<std::unique_ptr<RenderObject>> render_objects {};
        // Set up 8 cubes around the origin
        for ( unsigned char i { 0 }; i < 8; ++i ) {
            auto const r { static_cast<float>(i >> 2 & 1) };
            auto const g { static_cast<float>(i >> 1 & 1) };
            auto const b { static_cast<float>(i & 1) };
            MeshBuilder builder { MeshBuilder::generate_cube() };
            builder.m_colours = { 8, { r, g, b } };
            builder.generate_vertex_normals();
            render_objects.push_back(
                std::make_unique<RenderObject>( RenderObject::Opaque, builder.get_mesh(), &shader ) );
            RenderObject & object { *render_objects.back() };
            object.translate( { r - 0.5, g - 0.5, b - 0.5 } );
            object.scale( 0.3 );
            renderer.register_object( object );
        }

        // Create a camera object and attach it to the shader
        glm::vec3 const & camera_position { 0.f, 0.f, -3.f };
        glm::vec3 const & camera_target { 0.f, 0.f, 0.f };
        Camera camera { camera_position, camera_target, &shader };
        camera.set_free_view( window.get_input_manager() );

        shader.set_uniform( "model", glm::identity<glm::mat4>() );
        shader.set_uniform( "projection", glm::perspective( glm::radians( 45.f ), 1200.f / 800.f, 0.1f, 100.f ) );

        // Main program loop
        while ( !window.is_closing() ) {
            Time::loop_start();

            // object.translate( { 0.f, 0.f, -0.01f } );

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
