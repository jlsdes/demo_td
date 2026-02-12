#ifndef DEMO_TD_CONTROLLER_HPP
#define DEMO_TD_CONTROLLER_HPP

#include "system.hpp"

#include "graphics/input_manager.hpp"


class Window;


/** Handles (most) player input, which it receives through GLFW's callback functions. */
class Controller : public System {
public:
    Controller( ECS * ecs, Window & window );
    ~Controller() override;

    void run() override;

private:
    InputManager & m_input_manager;

    unsigned int m_mouse_callback;
};


#endif //DEMO_TD_CONTROLLER_HPP