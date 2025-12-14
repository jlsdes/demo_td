#ifndef DEMO_TD_CAMERA_HPP
#define DEMO_TD_CAMERA_HPP

#include "input_manager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <map>


class GraphicsShader;


/** The directions the camera can move in. */
enum Direction {
    Forward = 0,
    Backward = 1,
    Right = 2,
    Left = 3,
    Up = 4,
    Down = 5
};


/** The camera controlled by the user. Objects of this class will set the 'view' matrix uniform of the graphics shader
 *  they're attached to. */
class Camera {
public:
    /** Creates a new camera at the given position, looking at the given target position. This function assumes that the
     *  upward vector is always (0, 1, 0). */
    Camera( glm::vec3 const & position,
            glm::vec3 const & target,
            GraphicsShader * shader );

    /** Destructor. */
    ~Camera() = default;

    /** Sets the position of the camera, optionally while changing the direction it's facing as well. */
    void set_position( glm::vec3 const & position );

    /** Sets the rotation of the camera. */
    void set_rotation( glm::vec3 const & look_in_direction );
    void set_rotation( float yaw, float pitch );

    /** Moves the camera in the given direction, for a distance derived from its speed. */
    void translate( glm::vec3 const & direction );

    /** Rotates the camera toward the given direction (which is relative to the camera's current forward direction),
     *  depending on the camera's speed. */
    void rotate( glm::vec2 const & direction );

    /** Updates the view matrix in the attached shader. */
    void update();

    /** Sets the camera to free view mode, which lets the user move around freely.
     * @param input_manager The current window's InputManager object that the camera wants its callback functions to. */
    void set_free_view( InputManager & input_manager );

    /** Toggles whether the camera should move in one of the main directions (enumerated in Direction), depending on the
     *  specific key and whether it's being pressed or released. This function is used as a callback function registered
     *  to an InputManager object. */
    void toggle_movement( int key, int action );
    // TODO Separate ^this^ into a subclass so multiple types of cameras can exist or something that achieves this

private:
    /// The current position of the camera.
    glm::vec3 m_position;
    /// The yaw and pitch of the camera, i.e. horizontal and vertical rotation resp.
    float m_yaw;    // TODO Change to quaternions
    float m_pitch;
    /// Some vectors related to the current rotation of the camera.
    glm::vec3 m_forward;
    glm::vec3 m_right;
    glm::vec3 m_up;
    /// The (vertex) shader this camera is attached to.
    GraphicsShader * m_shader;

    /// For the camera in 'free-view' mode
    /// Toggles for keeping track of whether the user is currently holding down a movement key.
    std::array<bool, 6> m_directions;
    /// A mapping of keys to directions
    std::map<int, unsigned int> m_controls;
};


#endif //DEMO_TD_CAMERA_HPP
