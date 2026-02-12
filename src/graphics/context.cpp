#include "context.hpp"
#include "camera.hpp"
#include "entity_component_system.hpp"
#include "window.hpp"

#include "component/drawable.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "component/tower_data.hpp"

#include "system/controller.hpp"
#include "system/movement.hpp"
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

// TODO move these somewhere better (config perhaps?)
// Camera initialisation constants
glm::vec3 constexpr initial_position { -3.f, 0.f, 0.f };
glm::vec3 constexpr initial_target { 0.f, 0.f, 0.f };
Location const initial_location {
    .position = initial_position,
    .orientation = glm::quatLookAt( initial_target - initial_position, glm::vec3 { 0.f, 1.f, 0.f } )
};


TopContext::TopContext() : Context { nullptr }, m_window { nullptr },
                           m_camera { std::make_unique<Camera>( initial_position, initial_target ) },
                           m_ecs { std::make_unique<ECS>() } {
    initialise_glfw();
    m_window = std::make_unique<Window>();
    initialise_glad();

    m_ecs->components.create_store<Drawable>();
    m_ecs->components.create_store<Location>();

    m_ecs->systems.insert_system( std::make_unique<Renderer>( m_ecs.get(), *m_window, *m_camera ), Render );
    m_ecs->systems.insert_system( std::make_unique<Movement>( m_ecs.get() ), General );
    m_ecs->systems.insert_system( std::make_unique<Controller>( m_ecs.get(), *m_window ), General );

    EntityID const camera_entity { m_ecs->entities.create() };
    m_ecs->components.insert_component( camera_entity, initial_location );
    m_ecs->entities.set_entity_name( "camera", camera_entity );
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
