#include "camera.hpp"
#include "shader.hpp"
#include "utils/config.hpp"
#include "utils/time.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>


float constexpr sensitivity_multiplier { 0.004f }; ///< Allows the sensitivity default to be a nice round 1.
glm::vec3 constexpr world_up { 0.f, 1.f, 0.f };


/// Computes the right-facing vector for a given forward-facing vector.
inline glm::vec3 compute_right( glm::vec3 const & forward ) {
    return glm::normalize( glm::cross( forward, world_up ) );
}

/// Computes the upward-facing vector for given forward- and right-facing vectors.
inline glm::vec3 compute_up( glm::vec3 const & forward, glm::vec3 const & right ) {
    return glm::normalize( glm::cross( right, forward ) );
}


Camera::Camera( glm::vec3 const & position, glm::vec3 const & target )
    : m_position { position }, m_yaw {}, m_pitch {}, m_forward { glm::normalize( target - position ) },
      m_right { compute_right( m_forward ) }, m_up { compute_up( m_forward, m_right ) }, m_movement {},
      m_speed { 1.f }, m_sensitivity { 1.f } {
    // Try to load attributes from the config, and if it fails just continue with the default values
    try {
        set_speed( Config::get<float>( "Camera", "speed" ) );
    } catch ( std::out_of_range const & ) {}
    try {
        set_sensitivity( Config::get<float>( "Camera", "sensitivity" ) );
    } catch ( std::out_of_range const & ) {}

    set_rotation( target - position );
    update();
}

void Camera::set_position( glm::vec3 const & position ) {
    m_position = position;
}

void Camera::set_rotation( glm::vec3 const & look_in_direction ) {
    m_forward = glm::normalize( look_in_direction );
    m_right = compute_right( m_forward );
    m_up = compute_up( m_forward, m_right );

    m_pitch = std::asin( m_forward.y );
    // Using only one of the x and z components does not give enough information to determine the yaw with certainty
    // They can be used to compute either the sine or the cosine (resp.), which together do determine the correct yaw
    float const yaw_sin = m_forward.z / std::cos( m_pitch );
    float const yaw_cos = m_forward.x / std::cos( m_pitch );
    m_yaw = (yaw_sin >= 0 ? 0.f : glm::pi<float>()) + std::acos( yaw_cos );
}

void Camera::set_rotation( float const yaw, float const pitch ) {
    m_yaw = yaw;
    float constexpr upper { glm::half_pi<float>() - 0.01f };
    m_pitch = std::clamp( pitch, -upper, upper );

    m_forward = glm::normalize( glm::vec3 {
        std::cos( m_yaw ) * std::cos( m_pitch ),
        std::sin( m_pitch ),
        std::sin( m_yaw ) * std::cos( m_pitch )
    } );
    m_right = compute_right( m_forward );
    m_up = compute_up( m_forward, m_right );
}

void Camera::translate( glm::vec3 const & direction ) {
    auto const elapsed_time { static_cast<float>(Time::get_elapsed_time()) };
    auto const sprint { m_movement[Sprint] ? 2.f : 1.f };
    m_position += m_speed * elapsed_time * sprint * glm::normalize( direction );
}

void Camera::rotate( glm::vec2 const & mouse_offset ) {
    glm::vec2 const offset { mouse_offset * m_sensitivity * sensitivity_multiplier };
    m_yaw += offset.x;
    m_pitch -= offset.y;
    set_rotation( m_yaw, m_pitch );
}

glm::vec3 Camera::get_forward() const {
    return m_forward;
}

glm::vec3 Camera::get_right() const {
    return m_right;
}

glm::vec3 Camera::get_up() const {
    return m_up;
}

float Camera::get_speed() const {
    return m_speed;
}

void Camera::set_speed( float const speed ) {
    assert( speed >= 0.f );
    m_speed = speed;
}

float Camera::get_sensitivity() const {
    return m_sensitivity;
}

void Camera::set_sensitivity( float const sensitivity ) {
    assert( sensitivity >= 0.f );
    m_sensitivity = sensitivity;
}

void Camera::update() {
    // Moving forward and backward at the same time simply cancel each other out
    // 'bool != bool' is equivalent to ^ (xor), but ^ does not return a bool
    bool const moving_bf { m_movement[Forward] != m_movement[Backward] };
    bool const moving_rl { m_movement[Right] != m_movement[Left] };
    bool const moving_ud { m_movement[Up] != m_movement[Down] };

    if ( moving_bf || moving_rl || moving_ud ) {
        glm::vec3 direction { 0.f };
        if ( moving_bf ) direction += m_movement[Forward] ? m_forward : -m_forward;
        if ( moving_rl ) direction += m_movement[Right] ? m_right : -m_right;
        if ( moving_ud ) direction += m_movement[Up] ? world_up : -world_up;
        translate( direction );
    }
}

void Camera::update_shader( Shader const & shader ) const {
    shader.set_uniform( "camera_position", m_position );
    shader.set_uniform( "view", glm::lookAt( m_position, m_position + m_forward, m_up ) );
}

void Camera::toggle_movement( Action const action, bool const is_pressing ) {
    assert( action < NumberActions );
    m_movement[action] = is_pressing;
}
