#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "system/system.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"


/** Renders all drawable components. */
class Renderer : public System {
public:
    Renderer( ECS * ecs, Window & window, Camera & camera );
    ~Renderer() override = default;

    void run() override;

private:
    Window & m_window;
    Camera & m_camera;
    ShaderStore m_shaders;
};


#endif //DEMO_TD_RENDERER_HPP
