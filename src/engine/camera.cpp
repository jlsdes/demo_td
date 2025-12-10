#include "camera.hpp"
#include "shader.hpp"

#include <glm/gtc/matrix_transform.hpp>


/** Returns an upward pointing vector relative to the direction the camera is facing. */
glm::vec3 get_up( glm::vec3 const & forward )
{
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
      m_up { glm::normalize( glm::cross( m_forward, m_right ) ) }, m_shader { shader }
{
    update();
}

void Camera::set_position( glm::vec3 const & position )
{
    m_position = position;
    update();
}

void Camera::set_position( glm::vec3 const & position,
                           glm::vec3 const & target )
{
    m_position = position;
    m_forward = glm::normalize( target - m_position );
    m_right = glm::normalize( glm::cross( m_forward, world_up ) );
    m_up = glm::normalize( glm::cross( m_forward, m_right ) );
    update();
}

void Camera::move( glm::vec3 const & direction )
{
    // TODO take elapsed time into account
    m_position += camera_speed * glm::normalize( direction );
    update();
}

void Camera::update() const
{
    m_shader->set_uniform( "view", glm::lookAt( m_position, m_position + m_forward, m_up ) );
}
