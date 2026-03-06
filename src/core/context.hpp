#ifndef DEMO_TD_CONTEXT_HPP
#define DEMO_TD_CONTEXT_HPP

#include <memory>


class Camera;
class Window;
struct ECS;


/** Contexts initialise any entities and systems that are required to run the associated code. When the context object
 *  is destroyed, those entities and systems are removed again.
 *  These contexts generally function within a hierarchy. For example, the TopContext object handles general program
 *  initialisation such as window creation for example, and then the LevelContext initialises level-specific stuff later
 *  on (which I haven't implemented at the time of writing). Contexts can use ancestors' functionality, and they can
 *  either extend or replace functionality as well. */
class Context {
protected:
    explicit Context( Context const * parent );

public:
    virtual ~Context() = default;

    [[nodiscard]] Context const * get_parent() const;

    [[nodiscard]] virtual ECS * get_ecs() const { return m_parent ? m_parent->get_ecs() : nullptr; }

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
    [[nodiscard]] Window * get_window() const { return m_window.get(); }

    /** Does nothing, these systems probably shouldn't be disabled. */
    void disable_systems() override {}
    void enable_systems() override {}

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<ECS> m_ecs;
};


class LevelContext : public Context {
public:
    explicit LevelContext( Context const * parent );
    ~LevelContext() override;

    void disable_systems() override;
    void enable_systems() override;

private:
};


class MenuContext : public Context {
public:
    explicit MenuContext( Context const * parent );
    ~MenuContext() override;

    void disable_systems() override;
    void enable_systems() override;

private:
};


#endif //DEMO_TD_CONTEXT_HPP