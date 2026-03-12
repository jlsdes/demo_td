#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "system.hpp"
#include "component/entity_type.hpp"
#include "core/shader.hpp"
#include "entity/entity.hpp"

#include <memory>


class SubRenderer;


/** Renders all drawable components. */
class Renderer : public System {
public:
    Renderer( Context const & context, Window & window, Camera & camera );
    ~Renderer() override;

    void run() override;

private:
    ShaderStore m_shaders;

    std::unique_ptr<SubRenderer> m_sub_renderers[EntityType::NrTypes];
};


/** Base class for sub-renderers that handle specific entity type rendering. */
class SubRenderer {
public:
    SubRenderer( Context const & context, ShaderStore & shaders ) : m_context { context }, m_shaders { shaders } {}
    virtual ~SubRenderer() = default;

    /** Called at the start of every render loop; does nothing unless overridden by a derived class. */
    virtual void start() {}
    /** Called for every entity that should be handled by the sub-renderer; does nothing unless overridden. */
    virtual void draw( EntityID entity ) {}
    /** Called after every render loop; does nothing unless overridden. */
    virtual void finish() {}

protected:
    Context const & m_context;
    ShaderStore & m_shaders;
};


#endif //DEMO_TD_RENDERER_HPP
