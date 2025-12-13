#include "camera.hpp"
#include "shader.hpp"
#include "time.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <ranges>
#include <set>
#include <stdexcept>


// Camera parameters
float constexpr camera_speed { 1.f };
float constexpr camera_sensitivity { 0.1f };
glm::vec3 constexpr world_up { 0.f, 1.f, 0.f };


/// Computes the right-facing vector for a given forward-facing vector.
inline glm::vec3 compute_right( glm::vec3 const & forward ) {
    return glm::normalize( glm::cross( forward, world_up ) );
}

/// Computes the upward-facing vector for given forward- and right-facing vectors.
inline glm::vec3 compute_up( glm::vec3 const & forward, glm::vec3 const & right ) {
    return glm::normalize( glm::cross( right, forward ) );
}


Camera::Camera( glm::vec3 const & position,
                glm::vec3 const & target,
                GraphicsShader * const shader )
    : m_position { position }, m_yaw { -90.f }, m_pitch { 0.f }, m_forward { glm::normalize( target - position ) },
      m_right { compute_right( m_forward ) }, m_up { compute_up( m_forward, m_right ) }, m_shader { shader },
      m_directions {}, m_controls {} {
    update();

    // Default keybinds for free-view camera
    m_controls[GLFW_KEY_W] = Direction::Forward;
    m_controls[GLFW_KEY_A] = Direction::Left;
    m_controls[GLFW_KEY_S] = Direction::Backward;
    m_controls[GLFW_KEY_D] = Direction::Right;
    m_controls[GLFW_KEY_LEFT_SHIFT] = Direction::Up;
    m_controls[GLFW_KEY_LEFT_CONTROL] = Direction::Down;
}

void Camera::set_position( glm::vec3 const & position ) {
    m_position = position;
}

void Camera::set_position( glm::vec3 const & position,
                           glm::vec3 const & target ) {
    set_position( position );
    set_rotation( target - position );
}

void Camera::set_rotation( glm::vec3 const & rotation ) {
    m_forward = glm::normalize( rotation );
    m_right = compute_right( m_forward );
    m_up = compute_up( m_forward, m_right );

    // TODO finish
    m_pitch = asin( m_pitch );
}

void Camera::set_rotation( float const yaw, float const pitch ) {
    m_yaw = yaw;
    m_pitch = std::clamp( pitch, -89.f, 89.f );

    m_forward = glm::normalize( glm::vec3 {
        cos( glm::radians( m_yaw ) ) * cos( glm::radians( m_pitch ) ),
        sin( glm::radians( m_pitch ) ),
        sin( glm::radians( m_yaw ) ) * cos( glm::radians( m_pitch ) )
    } );
    m_right = compute_right( m_forward );
    m_up = compute_up( m_forward, m_right );
}

void Camera::move( glm::vec3 const & direction ) {
    auto const elapsed_time { static_cast<float>(Time::get_elapsed_time()) };
    m_position += camera_speed * elapsed_time * glm::normalize( direction );
}

void Camera::rotate( glm::vec2 const & direction ) {
    static glm::vec2 previous_direction { 0.f, 0.f };
    static bool first_call { true };
    if ( first_call ) {
        previous_direction = direction;
        first_call = false;
        return;
    }

    glm::vec2 const offset { (direction - previous_direction) * camera_sensitivity };
    m_yaw += offset.x;
    m_pitch -= offset.y;
    set_rotation( m_yaw, m_pitch );
    previous_direction = direction;
}

void Camera::toggle_movement( int const key, int const action ) {
    assert( m_controls.contains( key ) );
    // GLFW_REPEAT is also a valid action, but we don't need it here
    if ( action != GLFW_PRESS && action != GLFW_RELEASE )
        return;
    m_directions.at( m_controls.at( key ) ) = action == GLFW_PRESS;
}

void Camera::update() {
    // Moving forward and backward at the same time simply cancel each other out
    // 'bool != bool' is equivalent to ^ (xor), but ^ does not return a bool
    bool const moving_bf { m_directions.at( Direction::Forward ) != m_directions.at( Direction::Backward ) };
    bool const moving_rl { m_directions.at( Direction::Right ) != m_directions.at( Direction::Left ) };
    bool const moving_ud { m_directions.at( Direction::Up ) != m_directions.at( Direction::Down ) };

    if ( moving_bf || moving_rl || moving_ud ) {
        glm::vec3 direction { 0.f };
        if ( moving_bf ) direction += m_directions.at( Direction::Forward ) ? m_forward : -m_forward;
        if ( moving_rl ) direction += m_directions.at( Direction::Right ) ? m_right : -m_right;
        if ( moving_ud ) direction += m_directions.at( Direction::Up ) ? m_up : -m_up;
        move( direction );
    }
    m_shader->set_uniform( "view", glm::lookAt( m_position, m_position + m_forward, m_up ) );
}

void Camera::set_free_view( InputManager & input_manager ) {
    // Create a set with all keys used for movement, and register them in one go because we can
    std::set<int> keys {};
    for ( int const & key : std::views::keys( m_controls ) )
        keys.emplace( key );
    input_manager.observe_keyboard( keys, [this]( int const key, int const action ) {
        this->toggle_movement( key, action );
    } );

    input_manager.observe_mouse( [this]( double const x_position, double const y_position ) {
        this->rotate( glm::vec2 { x_position, y_position } );
    } );
}
