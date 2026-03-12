#ifndef DEMO_TD_CONTROLLER_HPP
#define DEMO_TD_CONTROLLER_HPP

#include "system.hpp"

#include "core/camera.hpp"

#include <array>
#include <bitset>


class InputManager;
struct ECS;


/** Handles (most) player input, which it receives through GLFW's callback functions. */
class Controller : public System {
public:
    Controller( Context const & context, InputManager & input_manager, Camera & camera );
    ~Controller() override;

    void run() override;

private:
    void handle_camera_key( Camera::Action camera_action, int key_action ) const;
    void handle_cursor_movement( double x, double y ) const;
    void toggle_camera_mode();

    InputManager & m_input_manager;
    Camera & m_camera;

    enum Action {
        // The first values are reserved for the Camera::Action enum values
        CameraEnableRotate = Camera::NumberActions,
        CameraFreeView,
        CursorMove,
        NumberActions ///< Not a valid action; must remain the final enum value to indicate the total number of actions.
    };

    /// All registered callback IDs. The first few callbacks in this array are used for camera movement actions, and
    /// have indices that are the same as the values in the CameraAction enum. The other action(s) have indices that are
    /// defined above.
    std::array<unsigned int, NumberActions> m_callback_ids;

    enum Flag {
        IsCameraRotating,
        IsCameraFreeView,
        NumberFlags // Not a valid flag; must be the last enum value to correctly indicate the total number of flags
    };

    std::bitset<NumberFlags> m_flags;
};


#endif //DEMO_TD_CONTROLLER_HPP
