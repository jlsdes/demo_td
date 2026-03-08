#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "component/entity_type.hpp"
#include "core/camera.hpp"
#include "core/shader.hpp"
#include "core/window.hpp"
#include "entity/entity.hpp"
#include "system/system.hpp"


/** Renders all drawable components. */
class Renderer : public System {
public:
    Renderer( ECS * ecs, Window & window, Camera & camera );
    ~Renderer() override;

    void run() override;

private:
    void render_entity( EntityID entity, EntityType type );
    void render_tower( EntityID entity );

    Window & m_window;
    Camera & m_camera;
    ShaderStore m_shaders;

    class TileRenderer;

    TileRenderer * m_tile_renderer;
};


#endif //DEMO_TD_RENDERER_HPP
