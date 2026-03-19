#include "camera.hpp"
#include "shader.hpp"

#include <numbers>
#include <thread>

#include <glm/gtc/matrix_transform.hpp>


Camera::Camera()
        : m_view { glm::identity<glm::mat4>() },
          m_projection { glm::perspective( std::numbers::pi_v<float> / 4, 4.f / 3.f, 0.1f, 100.f ) },
          m_home_thread { std::this_thread::get_id() } {}

void Camera::set_view( Shader & shader ) const {
    shader.set_uniform( "view", m_view );
}

void Camera::set_projection( Shader & shader ) const {
    shader.set_uniform( "projection", m_projection );
}
