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
    Window & m_window;
    Camera & m_camera;
    ShaderStore m_shaders;

    std::unique_ptr<SubRenderer> m_sub_renderers[EntityType::NrTypes];

    friend class TileRenderer;
    friend class TowerRenderer;
};


/** Base class for sub-renderers that handle specific entity type rendering. */
class SubRenderer {
public:
    explicit SubRenderer( Renderer * parent = nullptr ) : m_parent { parent } {}
    virtual ~SubRenderer() = default;

    /** Called at the start of every render loop; does nothing unless overridden by a derived class. */
    virtual void start() {}
    /** Called for every entity that should be handled by the sub-renderer; does nothing unless overridden. */
    virtual void draw( EntityID entity ) {}
    /** Called after every render loop; does nothing unless overridden. */
    virtual void finish() {}

protected:
    Renderer * const m_parent;
};


#endif //DEMO_TD_RENDERER_HPP
