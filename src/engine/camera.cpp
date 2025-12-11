#include "camera.hpp"
#include "shader.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <ranges>
#include <set>

#include <iostream>


/** Returns an upward pointing vector relative to the direction the camera is facing. */
glm::vec3 get_up( glm::vec3 const & forward ) {
    glm::vec3 constexpr world_up { 0.f, 1.f, 0.f };
    glm::vec3 const right { glm::cross( world_up, forward ) };
    return glm::normalize( glm::cross( right, forward ) );
}


// Camera parameters
float constexpr camera_speed { 1.f };
glm::vec3 constexpr world_up { 0.f, 1.f, 0.f };


Camera::Camera( glm::vec3 const & position,
                glm::vec3 const & target,
                GraphicsShader * const shader )
    : m_position { position }, m_forward { glm::normalize( target - position ) },
      m_right { glm::normalize( glm::cross( m_forward, world_up ) ) },
      m_up { glm::normalize( glm::cross( m_forward, m_right ) ) }, m_shader { shader },
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
    update();
}

void Camera::set_position( glm::vec3 const & position,
                           glm::vec3 const & target ) {
    m_position = position;
    m_forward = glm::normalize( target - m_position );
    m_right = glm::normalize( glm::cross( m_forward, world_up ) );
    m_up = glm::normalize( glm::cross( m_forward, m_right ) );
    update();
}

void Camera::move( glm::vec3 const & direction ) {
    // TODO take elapsed time into account
    m_position += camera_speed * glm::normalize( direction );
}

void Camera::toggle_movement( int key, int action ) {
    if ( action != GLFW_PRESS && action != GLFW_RELEASE )
        return;
    if ( !m_controls.contains( key ) )
        return;
    m_directions.at( m_controls.at( key ) ) = action == GLFW_PRESS;
}

void Camera::update() {
    // Check the direction toggles for whether the camera needs to be moved
    auto const moving_bf { m_directions.at( Direction::Forward ) ^ m_directions.at( Direction::Backward ) };
    auto const moving_rl { m_directions.at( Direction::Right ) ^ m_directions.at( Direction::Left ) };
    auto const moving_ud { m_directions.at( Direction::Up ) ^ m_directions.at( Direction::Down ) };

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
    std::set<int> keys {};
    for ( int const & key : std::views::keys( m_controls ) )
        keys.emplace( key );

    input_manager.observe_keyboard( keys, [this]( int const key, int const action ) {
        this->toggle_movement( key, action );
    } );
}
