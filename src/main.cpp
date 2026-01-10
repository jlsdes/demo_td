#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/camera.hpp"
#include "engine/mesh.hpp"
#include "engine/mesh_builder.hpp"
#include "engine/model_object.hpp"
#include "engine/model_manager.hpp"
#include "engine/renderer.hpp"
#include "engine/shader.hpp"
#include "engine/view_object.hpp"
#include "engine/window.hpp"
#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"

#include <filesystem>
#include <random>
#include <thread>


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

void render_loop( Window const & window, Renderer const & renderer, Camera & camera ) {
    unsigned int counter { 0 };
    // GLFW handles FPS limiting if VSync is enabled, which it probably is
    while ( !window.is_closing() ) {
        window.clear();
        renderer.draw();
        camera.update();
        window.render();

        if ( ++counter > 120 )
            window.close();
    }
}

void game_loop( Window const & window ) {
    ModelManager model_manager {};
    ModelManager also_model_manager {};
    for ( unsigned int i { 0 }; i < 100; ++i ) {
        auto model { std::make_unique<ModelObject>( glm::vec3 { static_cast<float>(i), 0.f, 0.f } ) };
        model_manager.add_model( std::move( model ) );
        auto also_model { std::make_unique<ModelObject>( glm::vec3 { static_cast<float>(i), 0.f, 0.f } ) };
        also_model_manager.add_model( std::move( also_model ) );
    }

    double constexpr ticks_per_second { 100. };
    std::chrono::duration<double> constexpr tick_duration { 1. / ticks_per_second };

    double margin { 0. };
    unsigned int counter { 0 };
    while ( !window.is_closing() ) {
        auto const loop_start { std::chrono::steady_clock::now() };

        Time::loop_start();
        glfwPollEvents();
        model_manager.update_models();
        also_model_manager.update_models();

        // Adjust the sleeping duration slightly according to the accuracy of the previous iteration's duration
        auto const sleepy_time {
            tick_duration - std::chrono::duration<double> { margin > 0 ? margin : 0 }
        };
        auto wake_up_time { loop_start + sleepy_time };
        std::this_thread::sleep_until( wake_up_time );
        margin = Time::get_elapsed_time() - tick_duration.count();
        Log::debug( "Iteration ", counter++, "\t-- Tick length ", Time::get_elapsed_time(), "\t-- Margin ", margin );
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
                glDisable( GL_CULL_FACE );
            } else if ( action == GLFW_RELEASE ) {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                shader.set_uniform( "ambient_light", ambient_light );
                glEnable( GL_CULL_FACE );
            }
        } );

        // Create a camera object and attach it to the shader
        glm::vec3 const & camera_position { 0.f, 1.f, -3.f };
        glm::vec3 const & camera_target { 0.f, 0.f, 0.f };
        Camera camera { camera_position, camera_target, &shader };
        camera.set_free_view( window.get_input_manager() );

        Renderer renderer {};
        std::vector<std::unique_ptr<ViewObject>> render_objects {};

        std::random_device random_device {};
        std::uniform_real_distribution distribution( 0.5f, 1.f );
        auto const elevation { [&]( float, float ) { return distribution( random_device ); } };

        MeshBuilder builder { MeshBuilder::grid( 10.f, 10.f, 20, 20, elevation ) };
        builder.translate( camera_target );
        builder.m_colours = { builder.m_vertices.size(), glm::vec3 {} };

        for ( glm::vec3 & colour : builder.m_colours ) {
            colour.g = distribution( random_device );
        }

        ViewObject grid { ViewObject::Terrain, builder.get_mesh(), &shader };
        renderer.register_object( grid );

        // for ( unsigned char i { 0 }; i < 8; ++i ) {
        //     glm::vec3 offset { i & 4 ? -0.5f : 0.5f, i & 2 ? -0.5f : 0.5f, i & 1 ? -0.5f : 0.5f };
        //
        //     builder.m_colours = { builder.m_vertices.size(), offset + glm::vec3 { 0.5f } };
        //     builder.translate( offset );
        //     render_objects.push_back(
        //         std::make_unique<RenderObject>( RenderObject::Opaque, builder.get_mesh(), &shader ) );
        //     builder.translate( -offset );
        //
        //     RenderObject & object { *render_objects.back() };
        //     renderer.register_object( object );
        // }

        float constexpr fov { glm::quarter_pi<float>() }; // 45 degrees
        shader.set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );

        std::thread game_thread { game_loop, std::ref( window ) };
        render_loop( window, renderer, camera );
        game_thread.join();
    }
    glfwTerminate();
    return 0;
}
