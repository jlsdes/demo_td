#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "system/system.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"

#include <memory>


/** Renders all drawable components. */
class Renderer : public System {
public:
    explicit Renderer( Window & window );
    ~Renderer() override = default;

    void run( EntityManager const & entities, ComponentManager & components ) override;

    Shader & get_shader( unsigned int shader_id = 0 );

private:
    Window & m_window;
    std::unique_ptr<Camera> m_camera;
    ShaderStore m_shaders;
};


#endif //DEMO_TD_RENDERER_HPP
