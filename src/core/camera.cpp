#include "camera.hpp"
#include "shader.hpp"
#include "window_context.hpp"

#include <numbers>

#include <glm/gtc/matrix_transform.hpp>

#include <lib/gl.hpp>


Camera::Camera() : m_view { glm::identity<glm::mat4>() }, m_projection {} {}

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
}
