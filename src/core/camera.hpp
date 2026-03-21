#pragma once

#include <glm/glm.hpp>


class Shader;
struct WindowContext;


class Camera {
public:
    Camera();

    void update_view( Shader & shader ) const;
    void update_projection( Shader & shader ) const;

private:
    glm::mat4 m_view;
    glm::mat4 m_projection;

    glm::vec3 m_forward;

    glm::vec3 m_moving;

    /// Initialises the camera after the entire window context has been created.
    /// Called by Window Context.
    void initialise( WindowContext & context );

    friend struct WindowContext;
};
