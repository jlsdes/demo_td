#ifndef DEMO_TD_CONTEXT_HPP
#define DEMO_TD_CONTEXT_HPP

#include <memory>


class Camera;
class Window;

class EntityManager;
class ComponentManager;
class SystemManager;

struct ECS;


/** Contexts initialise any entities and systems that are required to run the associated code. When the context object
 *  is destroyed, those entities and systems are removed again.
 *  These contexts generally function within a hierarchy. For example, the TopContext object handles general program
 *  initialisation such as window creation for example, and then the LevelContext initialises level-specific stuff later
 *  on (which I haven't implemented at the time of writing). Contexts can use ancestors' functionality, and they can
 *  either extend or replace functionality as well. */
struct Context {
    EntityManager * entities;
    ComponentManager * components;
    SystemManager * systems;
    ECS * ecs; // TODO remove

    Window * window;
    Camera * camera;

    explicit Context( Context const * parent = nullptr );
    virtual ~Context() = default;

private:
    Context const * const m_parent;
};


/** Manages the top level context, required by all other parts of the program. */
class TopContext : public Context {
public:
    TopContext();
    ~TopContext() override;

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<ECS> m_ecs;
};


class LevelContext : public Context {
public:
    explicit LevelContext( Context const * parent );
    ~LevelContext() override;

private:
};


class MenuContext : public Context {
public:
    explicit MenuContext( Context const * parent );
    ~MenuContext() override;

private:
};


#endif //DEMO_TD_CONTEXT_HPP