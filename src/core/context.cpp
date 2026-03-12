#include "context.hpp"
#include "camera.hpp"
#include "entity_component_system.hpp"
#include "window.hpp"

#include "component/entity_type.hpp"
#include "component/drawable.hpp"
#include "component/location.hpp"
#include "component/terrain_tile.hpp"
#include "component/tower_data.hpp"

#include "system/controller.hpp"
#include "system/movement.hpp"
#include "system/renderer.hpp"
#include "system/tile_manager.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>


// TODO move these somewhere better (config perhaps?)
// Camera initialisation constants
glm::vec3 constexpr initial_position { -3.f, 0.f, 0.f };
glm::vec3 constexpr initial_target { 0.f, 0.f, 0.f };


void initialise_glfw() {
    if ( not glfwInit() ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLFW" );
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 ); // Enable MSAA (anti-aliasing)

    // Experiment: transparent window
    // glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE );
}


void initialise_glad() {
    if ( not gladLoadGL( glfwGetProcAddress ) ) {
        glfwTerminate();
        throw std::runtime_error( "Failed to initialise GLAD." );
    }
    // If the window is transparent, make it fully transparent, otherwise use a dark grey background
    if ( glfwGetWindowAttrib( glfwGetCurrentContext(), GLFW_TRANSPARENT_FRAMEBUFFER ) )
        glClearColor( 0.f, 0.f, 0.f, 0.f );
    else
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
}


Context::Context( Context const * const parent ) : entities { parent ? parent->entities : nullptr },
                                                   components { parent ? parent->components : nullptr },
                                                   systems { parent ? parent->systems : nullptr },
                                                   ecs { parent ? parent->ecs : nullptr },
                                                   window { parent ? parent->window : nullptr },
                                                   camera { parent ? parent->camera : nullptr },
                                                   m_parent { parent } {}


TopContext::TopContext() : Context { nullptr }, m_window { nullptr },
                           m_camera { std::make_unique<Camera>( initial_position, initial_target ) },
                           m_ecs { std::make_unique<ECS>() } {
    initialise_glfw();
    m_window = std::make_unique<Window>();
    initialise_glad();
    InputManager & input_manager { m_window->get_input_manager() };

    entities = &m_ecs->entities;
    components = &m_ecs->components;
    systems = &m_ecs->systems;
    ecs = m_ecs.get();
    window = m_window.get();
    camera = m_camera.get();

    // When using the default swap interval, where GLFW synchronises the framerate with the screen's refresh rate, I get
    // quite a lot of dropped frames when moving the mouse around (only when using the default cursor mode). I'm not
    // sure why this happens, and why it doesn't happen when setting the cursor mode to GLFW_CURSOR_DISABLED, but it
    // does. As such, I've decided to just handle the frame rate limiting myself.
    glfwSwapInterval( 0 );

    components->create_store<Drawable>();
    components->create_store<EntityType>();
    components->create_store<Location>();

    systems->insert_system( std::make_unique<Renderer>( m_ecs.get(), *m_window, *m_camera ), Render );
    systems->insert_system( std::make_unique<Movement>( m_ecs.get() ), General );
    systems->insert_system( std::make_unique<Controller>( m_ecs.get(), input_manager, *m_camera ), General );
}


TopContext::~TopContext() = default;


LevelContext::LevelContext( Context const * const parent ) : Context { parent } {
    assert( parent );
    components->create_store<TowerType>();
    components->create_store<TerrainTile>();

    systems->insert_system( std::make_unique<TileManager>( ecs ), Setup );
}


LevelContext::~LevelContext() {
    systems->remove_system<TileManager>();

    components->remove_store( components->get_type_id<TowerType>() );
    components->remove_store( components->get_type_id<TerrainTile>() );
}


MenuContext::MenuContext( Context const * const parent ) : Context { parent } {
    assert( parent );
}


MenuContext::~MenuContext() = default;
