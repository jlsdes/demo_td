#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/camera.hpp"
#include "engine/controller_manager.hpp"
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
#include <latch>


std::filesystem::path get_main_dir() {
    return (std::filesystem::path( __FILE__ ) / "../../").lexically_normal();
}

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

void render_thread( Window & window, std::latch & initialisation_latch ) {
    // Find and build the main graphics shader
    auto const vertex_shader { get_main_dir() / Config::get<std::string>( "Shader", "vertex_shader" ) };
    auto const fragment_shader { get_main_dir() / Config::get<std::string>( "Shader", "fragment_shader" ) };
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

    auto builder { MeshBuilder::sphere( 10 ) };
    builder.transform( glm::identity<glm::mat3>() * 0.3f );
    for ( unsigned char i { 0 }; i < 8; ++i ) {
        glm::vec3 offset { i & 4 ? -0.5f : 0.5f, i & 2 ? -0.5f : 0.5f, i & 1 ? -0.5f : 0.5f };

        builder.m_colours = { builder.m_vertices.size(), offset + glm::vec3 { 0.5f } };
        builder.translate( offset );
        render_objects.push_back( std::make_unique<ViewObject>( ViewObject::Opaque, builder.get_mesh(), &shader ) );
        builder.translate( -offset );

        ViewObject & object { *render_objects.back() };
        renderer.register_object( object );
    }

    float constexpr fov { glm::quarter_pi<float>() }; // 45 degrees
    shader.set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );

    // Wait until the other thread is ready as well
    initialisation_latch.arrive_and_wait();

    // GLFW handles FPS limiting if VSync is enabled, which it probably is
    while ( !window.is_closing() ) {
        window.clear();
        renderer.draw();
        camera.update();
        window.render();
    }
}

struct TempController : public ControllerObject {
    unsigned int id;
    unsigned int counter;

    explicit TempController( unsigned int const id ) : id { id }, counter { 0 } {}

    void update() override { ++counter;}
};

void game_thread( Window const & window, std::latch & initialisation_latch ) {
    // Set up some stuff for some basic testing
    ModelManager model_manager {};
    ControllerManager controller_manager {};

    for ( unsigned int i { 0 }; i < 100; ++i ) {
        auto model { std::make_unique<ModelObject>( glm::vec3 { static_cast<float>(i), 0.f, 0.f } ) };
        model_manager.push( std::move( model ) );
        controller_manager.push( std::make_unique<TempController>( i ) );
    }
    controller_manager.update();

    // Wait until the other thread is ready as well
    initialisation_latch.arrive_and_wait();

    double constexpr tick_duration { 1. / 100. };
    double margin { 0. };
    while ( !window.is_closing() ) {
        double const loop_start { Time::loop_start() };
        glfwPollEvents();
        model_manager.update();

        // The computations are likely to be done before the next game tick, so this thread needs to sleep briefly
        // At the time of implementation, subtracting the previous margin of error twice seems to be quite accurate
        // TODO verify this when actual computations are happening
        double const time_left { tick_duration - (Time::get_time() - loop_start) };
        double const sleep_time { time_left - (margin > 0 ? 2 * margin : 0) };
        // Log::debug( "Sleeping ", sleep_time, "s\t; Elapsed time ", Time::get_elapsed_time() );
        std::this_thread::sleep_for( std::chrono::duration<double> { sleep_time } );
        margin = Time::get_elapsed_time() - tick_duration;
    }
}

int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    Window window {};

    std::latch initialisation_latch { 2 };

    std::thread other_thread { game_thread, std::ref( window ), std::ref( initialisation_latch ) };
    render_thread( window, initialisation_latch );
    other_thread.join();

    glfwTerminate();
    return 0;
}
