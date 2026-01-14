#include "camera.hpp"
#include "engine.hpp"
#include "entity_factory.hpp"
#include "mesh_builder.hpp"
#include "shader.hpp"
#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <numbers>
#include <string>


template <typename T>
bool pop_manager( std::vector<std::unique_ptr<T>> & container, T const * manager, std::string const & name ) {
    for ( auto iterator { container.cbegin() }; iterator != container.cend(); ++iterator ) {
        if ( iterator->get() != manager )
            continue;
        container.erase( iterator );
        return true;
    }
    Log::error( "Attempted to remove a ", name, " that couldn't be found." );
    return false;
}

void initialise_glfw() {
    if ( not glfwInit() ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLFW" );
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 ); // Enable MSAA (anti-aliasing)
}

void initialise_glad() {
    if ( not gladLoadGL( glfwGetProcAddress ) ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLAD." );
    }
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
}

Engine::Engine() : m_window { nullptr }, m_models {}, m_views {}, m_controllers {}, m_game_thread {},
                   m_initialisation_latch { 2 } {
    static bool initialised = false;

    if ( not initialised )
        initialise_glfw();
    m_window = std::make_unique<Window>();
    if ( not initialised )
        initialise_glad();

    if ( not initialised ) {
        m_models.emplace_back( std::make_unique<ModelManager>() );
        m_views.emplace_back( std::make_unique<ViewManager>() );
        m_controllers.emplace_back( std::make_unique<ControllerManager>() );
        EntityFactory & factory { EntityFactory::get_instance() };
        factory.initialise( m_models.back().get(), m_views.back().get(), m_controllers.back().get() );
    }

    initialised = true;
}

void Engine::push_model_manager( std::unique_ptr<ModelManager> && model_manager ) {
    m_models.emplace_back( std::move( model_manager ) );
}

bool Engine::pop_model_manager( ModelManager const * model_manager ) {
    return pop_manager( m_models, model_manager, "ModelManager" );
}

void Engine::push_view_manager( std::unique_ptr<ViewManager> && view_manager ) {
    m_views.emplace_back( std::move( view_manager ) );
}

bool Engine::pop_view_manager( ViewManager const * view_manager ) {
    return pop_manager( m_views, view_manager, "Renderer" );
}

void Engine::push_controller_manager( std::unique_ptr<ControllerManager> && controller_manager ) {
    m_controllers.emplace_back( std::move( controller_manager ) );
}

bool Engine::pop_controller_manager( ControllerManager const * controller_manager ) {
    return pop_manager( m_controllers, controller_manager, "ControllerManager" );
}

struct TempController : public ControllerObject {
    unsigned int id;
    unsigned int counter;

    explicit TempController( unsigned int const id ) : id { id }, counter { 0 } {}

    void update() override { ++counter; }
};

void Engine::game_thread() {
    // Set up some stuff for some basic testing
    push_model_manager( std::make_unique<ModelManager>() );
    push_controller_manager( std::make_unique<ControllerManager>() );

    for ( unsigned int i { 0 }; i < 100; ++i ) {
        auto model { std::make_unique<ModelObject>( glm::vec3 { static_cast<float>(i), 0.f, 0.f } ) };
        m_models.at( 0 )->push( std::move( model ) );
        m_controllers.at( 0 )->push( std::make_unique<TempController>( i ) );
    }

    // Wait until the other thread is ready as well
    m_initialisation_latch.arrive_and_wait();

    double constexpr tick_duration { 1. / 100. };
    double margin { 0. };
    while ( not m_window->is_closing() ) {
        double const loop_start { Time::loop_start() };
        glfwPollEvents();

        for ( auto const & controller_manager : m_controllers )
            controller_manager->update();
        for ( auto const & model_manager : m_models )
            model_manager->update();

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

void Engine::render_thread() {
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
    auto & input_manager { m_window->get_input_manager() };
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
    camera.set_free_view( m_window->get_input_manager() );

    auto builder { MeshBuilder::sphere( 10 ) };
    builder.transform( glm::identity<glm::mat3>() * 0.3f );
    for ( unsigned char i { 0 }; i < 8; ++i ) {
        glm::vec3 offset { i & 4 ? -0.5f : 0.5f, i & 2 ? -0.5f : 0.5f, i & 1 ? -0.5f : 0.5f };

        builder.m_colours = { builder.m_vertices.size(), offset + glm::vec3 { 0.5f } };
        builder.translate( offset );
        m_views.back().get()->push( std::make_unique<ViewObject>( ViewObject::Opaque, builder.get_mesh(), &shader ) );
        builder.translate( -offset );
    }

    float constexpr fov { std::numbers::pi_v<float> / 4.f }; // 45 degrees
    shader.set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );

    // Wait until the other thread is ready as well
    m_initialisation_latch.arrive_and_wait();

    // GLFW handles FPS limiting if VSync is enabled, which it probably is
    while ( not m_window->is_closing() ) {
        m_window->clear();

        for ( auto const & view_manager : m_views )
            view_manager->update();
        for ( auto const & view_manager : m_views )
            view_manager->draw();

        camera.update();
        m_window->render();
    }
}

void Engine::run() {
    m_game_thread = std::thread { &Engine::game_thread, this };
    render_thread(); // The main thread will act as the render thread
    m_game_thread.join();
}

Window & Engine::get_window() const {
    return *m_window;
}
