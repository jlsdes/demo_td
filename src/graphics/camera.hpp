#ifndef DEMO_TD_CAMERA_HPP
#define DEMO_TD_CAMERA_HPP

#include <glm/glm.hpp>


class GraphicsShader;


/** The camera controlled by the user. Objects of this class will set the 'view' matrix uniform of the graphics shader
 *  they're attached to. */
class Camera
{
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
    void set_position( glm::vec3 const & position, glm::vec3 const & target );

    /** Updates the view matrix in the attached shader. */
    void update() const;

private:
    /// The current position of the camera.
    glm::vec3 m_position;
    /// Some vectors related to the current rotation of the camera.
    glm::vec3 m_forward;
    glm::vec3 m_right;
    glm::vec3 m_up;
    /// The (vertex) shader this camera is attached to.
    GraphicsShader * m_shader;
};


#endif //DEMO_TD_CAMERA_HPP
