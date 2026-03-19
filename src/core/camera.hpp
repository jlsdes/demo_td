#pragma once

#include <thread>

#include <glm/glm.hpp>


class Shader;


class Camera {
public:
    Camera();

    void set_view( Shader & shader ) const;
    void set_projection( Shader & shader ) const;

private:
    glm::mat4 m_view;
    glm::mat4 m_projection;

    std::thread::id const m_home_thread;
};
