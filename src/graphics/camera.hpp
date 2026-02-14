#ifndef DEMO_TD_CAMERA_HPP
#define DEMO_TD_CAMERA_HPP

#include "input_manager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <bitset>
#include <map>


class Shader;


/// The keyboard-related actions that affect the camera.
enum CameraActions : unsigned char {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
    Sprint,
    NumberActions // Must remain the last value in this enum; not a valid enum value
};


/** The camera controlled by the user. Objects of this class will set the 'view' matrix uniform of the graphics shader
 *  they're attached to. */
class Camera {
public:
    /** Creates a new camera at the given position, looking at the given target position. This function assumes that the
     *  upward vector is always (0, 1, 0). */
    Camera( glm::vec3 const & position, glm::vec3 const & target );

    ~Camera() = default;

    /** Moves the camera to a specific position, or in a given direction. */
    void set_position( glm::vec3 const & position );
    void translate( glm::vec3 const & direction );

    /** Rotates the camera, either by setting the rotation, or adjusting the current rotation of the camera to be more
     *  towards the given mouse position. */
    void set_rotation( glm::vec3 const & look_in_direction );
    void set_rotation( float yaw, float pitch );
    void rotate( glm::vec2 const & mouse_position );

    /** Directional vectors related to the orientation of the camera. */
    [[nodiscard]] glm::vec3 get_forward() const;
    [[nodiscard]] glm::vec3 get_right() const;
    [[nodiscard]] glm::vec3 get_up() const;

    /** Updates the camera's internal data. */
    void update();
    /** Updates the given shader using the camera's (updated) data. */
    void update_shader( Shader const & shader ) const;

    /** Toggles whether the camera should move in one of the main directions (enumerated in Direction), depending on the
     *  specific key and whether it's being pressed or released. This function is used as a callback function registered
     *  to an InputManager object. */
    void toggle_movement( int key, int action );

private:
    /// The current position of the camera.
    glm::vec3 m_position;
    /// The yaw and pitch of the camera, i.e. horizontal and vertical rotation resp.
    float m_yaw;
    float m_pitch;
    /// Some vectors related to the current rotation of the camera.
    glm::vec3 m_forward;
    glm::vec3 m_right;
    glm::vec3 m_up;

    /// For the camera in 'free-view' mode
    /// Toggles for keeping track of whether the user is currently holding down a movement key.
    std::bitset<7> m_movement;
    /// A mapping of keys to directions
    std::map<int, unsigned int> m_controls;
};


#endif //DEMO_TD_CAMERA_HPP
