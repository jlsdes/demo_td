#ifndef DEMO_TD_CONTROLLER_HPP
#define DEMO_TD_CONTROLLER_HPP

#include "system.hpp"
#include "graphics/camera.hpp"
#include "graphics/input_manager.hpp"

#include <array>

#include <glad/gl.h>
#include <GLFW/glfw3.h>


class Window;


/** Handles (most) player input, which it receives through GLFW's callback functions. */
class Controller : public System {
public:
    Controller( ECS * ecs, Window & window, Camera & camera );
    ~Controller() override;

    void run() override;

    void camera_translation( int key, int action ) const;
    void camera_rotation( double x, double y ) const;

private:
    InputManager & m_input_manager;
    Camera & m_camera;

    unsigned int m_mouse_callback;

    enum Action : unsigned char {
        NoAction,
        MoveCamera,
        NumberActions ///< Not a valid action; must the final enum value so it indicates the number of valid actions
    };

    struct CallbackData {
        unsigned int id { 0 };
        Action action { NoAction };
    };

    std::array<CallbackData, GLFW_KEY_LAST + 1> m_key_callbacks;
};


#endif //DEMO_TD_CONTROLLER_HPP