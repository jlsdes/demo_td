#ifndef DEMO_TD_CONTROLLER_HPP
#define DEMO_TD_CONTROLLER_HPP

#include "system.hpp"
#include "graphics/camera.hpp"

#include <array>
#include <bitset>


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
    void handle_camera_rotation( double x, double y ) const;
    void toggle_camera_mode();

    InputManager & m_input_manager;
    Camera & m_camera;

    enum Action {
        // The first values are reserved for the Camera::Action enum values
        CameraEnableRotate = Camera::NumberActions,
        CameraRotate,
        CameraFreeView,
        NumberActions ///< Not a valid action; must remain the final enum value to indicate the total number of actions.
    };

    /// All registered callback IDs. The first few callbacks in this array are used for camera movement actions, and
    /// have indices that are the same as the values in the CameraAction enum. The other action(s) have indices that are
    /// defined above.
    std::array<unsigned int, NumberActions> m_callback_ids;

    enum Flag {
        IsCameraRotating,
        IsCameraFreeView,
    };

    std::bitset<2> m_flags;
};


#endif //DEMO_TD_CONTROLLER_HPP
