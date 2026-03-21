#include "camera.hpp"
#include "shader.hpp"
#include "window_context.hpp"

#include <numbers>
#include <print>

#include <glm/gtc/matrix_transform.hpp>

#include <lib/gl.hpp>


Camera::Camera()
        : m_view { glm::identity<glm::mat4>() }, m_projection {}, m_moving { 0.f }, m_forward { 0.f, 0.f, 1.f } {}

void Camera::update_view( Shader & shader ) const {
    shader.set_uniform( "view", m_view );
}

void Camera::update_projection( Shader & shader ) const {
    shader.set_uniform( "projection", m_projection );
}

void Camera::initialise( WindowContext & context ) {
    auto const [width, height] { context.window.get_size() };
    float const ratio { static_cast<float>( width ) / height };

    float constexpr fov { std::numbers::pi_v<float> / 4 };
    float constexpr near { 0.1f };
    float constexpr far { 100.f };

    m_projection = glm::perspective( fov, ratio, near, far );

    auto const move_callback { [this]( int const key, int, int const action, int ) {
        float modifier { action == GLFW_REPEAT ? 0.f : action == GLFW_PRESS ? 1.f : -1.f };

        switch ( key ) {
        case GLFW_KEY_W:
            m_moving += modifier * m_forward;
            break;
        case GLFW_KEY_S:
            m_moving -= modifier * m_forward;
            break;
        }
        std::println( "{} {} {}", m_moving.x, m_moving.y, m_moving.z );
    } };
    context.input_manager.add_observer( { move_callback, GLFW_KEY_W } );
    context.input_manager.add_observer( { move_callback, GLFW_KEY_S } );
}
