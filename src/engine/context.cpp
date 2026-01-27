#include "context.hpp"
#include "entity_component_system.hpp"
#include "window.hpp"

#include "component/drawable.hpp"
#include "component/position.hpp"

#include "system/renderer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>


Context::Context( Context const * const parent ) : m_parent { parent } {}

Context const * Context::get_parent() const {
    return m_parent;
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

TopContext::TopContext() : Context { nullptr }, m_ecs { std::make_unique<ECS>() }, m_window { nullptr } {
    initialise_glfw();
    m_window = std::make_unique<Window>();
    initialise_glad();

    m_ecs->components.create_store<Drawable>();
    m_ecs->components.create_store<Position>();

    m_ecs->systems.insert_system( std::make_unique<Renderer>( *m_window ), Render );
}

TopContext::~TopContext() = default;

LevelContext::LevelContext( Context const * const current ) : Context { current } {}

LevelContext::~LevelContext() = default;

MenuContext::MenuContext( Context const * const current ) : Context { current } {}

MenuContext::~MenuContext() = default;


/*
Params initialise() {
    Params params {};

    if ( not glfwInit() ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLFW" );
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 ); // Enable MSAA (anti-aliasing)

    params.window = std::make_unique<Window>();

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

    // Find and build the main graphics shader
    auto const vertex_shader { get_main_dir() / Config::get<std::string>( "Shader", "vertex_shader" ) };
    auto const fragment_shader { get_main_dir() / Config::get<std::string>( "Shader", "fragment_shader" ) };
    params.shader = std::make_unique<GraphicsShader>( vertex_shader.c_str(), fragment_shader.c_str() );
    params.shader->use();

    glm::vec3 constexpr ambient_light { 0.01f };
    params.shader->set_uniform( "ambient_light", ambient_light );
    params.shader->set_uniform( "sun_light", glm::vec3 { 1.f, 1.f, 1.f } );
    glm::vec3 constexpr sun_direction { -0.2f, 1.f, -0.5f };
    params.shader->set_uniform( "sun_direction", sun_direction );

    float constexpr fov { std::numbers::pi_v<float> / 4.f }; // 45 degrees
    params.shader->set_uniform( "projection", glm::perspective( fov, 1200.f / 800.f, 0.1f, 100.f ) );

    glm::vec3 constexpr position { -3.f, 0.f, 0.f };
    glm::vec3 constexpr target { 0.f, 0.f, 0.f };
    params.camera = std::make_unique<Camera>( position, target, params.shader.get() );

    params.camera->set_free_view( params.window->get_input_manager() );

    return params;
}
 */
