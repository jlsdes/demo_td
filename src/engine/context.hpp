#ifndef DEMO_TD_CONTEXT_HPP
#define DEMO_TD_CONTEXT_HPP

#include "entity_component_system.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "window.hpp"

#include <memory>


/** Contexts initialise any entities and systems that are required to run the associated code. When the context object
 *  is destroyed, those entities and systems are also removed again.
 *  These contexts generally function within a hierarchy. For example, the TopContext object handles general program
 *  initialisation such as OpenGL stuff, and then the LevelContext can later on initialise level-specific stuff (which I
 *  haven't implemented at the time of writing). Contexts can use ancestors' functionality, and they can either extend
 *  or replace functionality as well. */
class Context {
protected:
    explicit Context( Context const * parent );

public:
    virtual ~Context() = default;

    [[nodiscard]] Context const * get_parent() const;

    [[nodiscard]] virtual ECS * get_ecs() const { return m_parent ? m_parent->get_ecs() : nullptr; }
    [[nodiscard]] virtual Window * get_window() const { return m_parent ? m_parent->get_window() : nullptr; }
    [[nodiscard]] virtual Camera * get_camera() const { return m_parent ? m_parent->get_camera() : nullptr; }

    /** Disables all systems in this specific context, meaning that they won't be run anymore. */
    virtual void disable_systems() = 0;
    /** Enables all systems in this specific context. */
    virtual void enable_systems() = 0;

private:
    Context const * const m_parent;
};


/** Manages the top level context, required by all other parts of the program. */
class TopContext : public Context {
public:
    TopContext();
    ~TopContext() override;

    [[nodiscard]] ECS * get_ecs() const override { return m_ecs.get(); }
    [[nodiscard]] Window * get_window() const override { return m_window.get(); }
    [[nodiscard]] Camera * get_camera() const override { return m_camera.get(); }

private:
    std::unique_ptr<ECS> m_ecs;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Camera> m_camera;

    std::vector<Shader> m_shaders;
};


class LevelContext : public Context {
public:
    explicit LevelContext( Context const * current );
    ~LevelContext() override;

private:
};


class MenuContext : public Context {
public:
    explicit MenuContext( Context const * current );
    ~MenuContext() override;

private:
};


#endif //DEMO_TD_CONTEXT_HPP