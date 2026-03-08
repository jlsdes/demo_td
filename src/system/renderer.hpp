#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "component/entity_type.hpp"
#include "core/camera.hpp"
#include "core/shader.hpp"
#include "core/window.hpp"
#include "entity/entity.hpp"
#include "system/system.hpp"

#include <memory>


class SubRenderer;


/** Renders all drawable components. */
class Renderer : public System {
public:
    Renderer( ECS * ecs, Window & window, Camera & camera );
    ~Renderer() override;

    void run() override;

private:
    void render_tower( EntityID entity );

    Window & m_window;
    Camera & m_camera;
    ShaderStore m_shaders;

    std::unique_ptr<SubRenderer> m_sub_renderers[EntityType::NrTypes];

    friend class TileRenderer;
};


class SubRenderer {
public:
    explicit SubRenderer( Renderer * parent = nullptr ) : m_parent { parent } {}
    virtual ~SubRenderer() = default;

    virtual void start() {}
    virtual void update( EntityID entity ) {}
    virtual void finish() {}

protected:
    Renderer * const m_parent;

    friend class Renderer;
};


#endif //DEMO_TD_RENDERER_HPP
