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

    enum Action : unsigned char {
        // NoAction,
        CameraForward,
        CameraBackward,
        CameraRight,
        CameraLeft,
        CameraUp,
        CameraDown,
        CameraSprint,
        CameraKeyFirst = CameraForward,
        CameraKeyLast = CameraSprint,
        CameraRotate,
        NumberActions ///< Not a valid action; must be the final enum value so it indicates the number of valid actions.
    };

    std::array<unsigned int, NumberActions> m_callback_ids;
};


#endif //DEMO_TD_CONTROLLER_HPP