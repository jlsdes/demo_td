#include "context.hpp"
#include "entity_component_system.hpp"
#include "window.hpp"

#include "component/drawable.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "component/tower_data.hpp"

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
    m_ecs->components.create_store<Location>();

    m_ecs->systems.insert_system( std::make_unique<Renderer>( m_ecs.get(), *m_window ), Render );
}

TopContext::~TopContext() = default;

LevelContext::LevelContext( Context const * const parent ) : Context { parent } {
    assert( parent );
    parent->get_ecs()->components.create_store<TowerData>();
    parent->get_ecs()->components.create_store<TerrainTile>();
}

LevelContext::~LevelContext() {
    ECS * ecs { Context::get_ecs() };
    ecs->components.remove_store( ecs->components.get_type_id<TowerData>() );
    ecs->components.remove_store( ecs->components.get_type_id<TerrainTile>() );
}

void LevelContext::disable_systems() {}

void LevelContext::enable_systems() {}

MenuContext::MenuContext( Context const * const parent ) : Context { parent } {
    assert( parent );
}

MenuContext::~MenuContext() = default;

void MenuContext::disable_systems() {}

void MenuContext::enable_systems() {}
