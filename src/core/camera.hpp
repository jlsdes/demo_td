#pragma once

#include <glm/glm.hpp>


class Shader;


class Camera {
public:
    Camera();

    void update_view( Shader & shader ) const;
    void update_projection( Shader & shader ) const;

private:
    glm::mat4 m_view;
    glm::mat4 m_projection;
};
