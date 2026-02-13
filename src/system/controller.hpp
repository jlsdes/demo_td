#ifndef DEMO_TD_CONTROLLER_HPP
#define DEMO_TD_CONTROLLER_HPP

#include "system.hpp"
#include "graphics/camera.hpp"

#include <array>


class InputManager;


/** Handles (most) player input, which it receives through GLFW's callback functions. */
class Controller : public System {
public:
    Controller( ECS * ecs, InputManager & input_manager, Camera & camera );
    ~Controller() override;

    void run() override;

    void camera_translation( int key, int action ) const;
    void camera_rotation( double x, double y ) const;

private:
    InputManager & m_input_manager;
    Camera & m_camera;

    unsigned int m_mouse_callback;

    enum InputType : unsigned char {
        NoType, ///< Indicator for the absence of a keybind for an action.
        Cursor,
        Keyboard,
        MouseButton,
        Scroll
    };

    enum Action : unsigned char {
        NoAction,
        CameraForward,
        CameraBackward,
        CameraRight,
        CameraLeft,
        CameraUp,
        CameraDown,
        CameraSprint,
        CameraFirst = CameraForward, ///< Utility value for checking for/iterating over camera actions.
        CameraLast = CameraSprint, ///< Utility value for checking for/iterating over camera actions.
        NumberActions ///< Not a valid action; must the final enum value so it indicates the number of valid actions
    };

    struct CallbackData {
        unsigned int id { 0 }; ///< The callback ID as returned by the InputManager.
        InputType type { NoType };
    };

    std::array<CallbackData, NumberActions> m_keybinds;
};


#endif //DEMO_TD_CONTROLLER_HPP