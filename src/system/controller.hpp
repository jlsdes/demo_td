#ifndef DEMO_TD_CONTROLLER_HPP
#define DEMO_TD_CONTROLLER_HPP

#include "system.hpp"
#include "graphics/camera.hpp"

#include <array>


class InputManager;
struct ECS;


/** Handles (most) player input, which it receives through GLFW's callback functions. */
class Controller : public System {
public:
    Controller( ECS * ecs, InputManager & input_manager, Camera & camera );
    ~Controller() override;

    void run() override;

private:
    void handle_camera_key( Camera::Action action, bool is_pressing ) const;
    void camera_rotation( double x, double y ) const;

    InputManager & m_input_manager;
    Camera & m_camera;

    static unsigned char constexpr CameraEnableRotate { Camera::NumberActions };
    static unsigned char constexpr CameraRotate { CameraEnableRotate + 1 };
    static unsigned char constexpr NumberActions { CameraRotate + 1 };

    /// All registered callback IDs. The first few callbacks in this array are used for camera movement actions, and
    /// have indices that are the same as the values in the CameraAction enum. The other action(s) have indices that are
    /// defined above.
    std::array<unsigned int, NumberActions> m_callback_ids;

    bool m_is_camera_rotating;
};


#endif //DEMO_TD_CONTROLLER_HPP