#ifndef DEMO_TD_RENDER_OBJECT_HPP
#define DEMO_TD_RENDER_OBJECT_HPP

#include "mesh.hpp"

#include <glm/glm.hpp>


class Shader;


/** The (base) class for any object that can be drawn. */
class RenderObject {
public:
    /** Types of renderable stuff, in the order in which they'll be drawn. */
    enum Type : unsigned int {
        Ui,
        Opaque,
        Terrain,
        Skybox,
        Transparent
    };

    /** Constructor and destructor. */
    RenderObject( Type type, Mesh && mesh, Shader * shader );
    ~RenderObject() = default;

    /** Draws the object on the next frame, if possible. */
    void draw() const;

    /** Toggles whether the object should actually be rendered. */
    void hide();
    void show();

    /** Some basic transformations. */
    void translate( glm::vec3 const & translation );
    void rotate( glm::vec3 const & axis, float angle );
    void scale( glm::vec3 const & scale );
    void scale( float scale );

private:
    /// The type of object this is.
    Type m_type;

    /// The main components for drawing this object.
    Mesh m_mesh;
    Shader * m_shader;

    /// The transformation matrix defining where and how the object is located.
    glm::mat4 m_transform;

    /// A toggle that can be used to skip this object during rendering.
    bool m_hidden { false };
};


#endif //DEMO_TD_RENDER_OBJECT_HPP
