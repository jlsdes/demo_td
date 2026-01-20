#ifndef DEMO_TD_RENDER_OBJECT_HPP
#define DEMO_TD_RENDER_OBJECT_HPP

#include "mesh.hpp"
#include "utils/manager.hpp"

#include <glm/glm.hpp>


class ModelObject;
class Shader;


/** The (base) class for any view object. View objects comprise any system that transforms the model's state into a
 *  rendered frame. Often these are entities being drawn, but also the camera class (will/) can be included in this. */
class ViewObject : public ManagedObject {
public:
    explicit ViewObject( ModelObject * model );
    ~ViewObject() override = default;

    /** Initialises any OpenGL data the ViewObject may need. This function must only be called in the render thread. */
    virtual void initialise();

    /** Updates and draws the object, CPU- and GPU-side respectively (more or less). */
    void update() override = 0;
    virtual void draw() const {}

protected:
    /// The model object that is being drawn.
    ModelObject * m_model;
};


/** Concept that requires types derived from the ViewObject class. */
template <typename Type>
concept ViewType = std::is_base_of_v<ViewObject, Type>;


/** Base class for ViewObject derivations that want to actually draw something. */
class VisibleObject : public ViewObject {
public:
    /** Types of renderable stuff, in the order in which they'll be drawn. */
    enum Type : unsigned int {
        Ui,
        Opaque,
        Terrain,
        Skybox,
        Transparent
    };

    VisibleObject( ModelObject * model, Type type, Mesh<ColourVertex> && mesh, Shader * shader );
    ~VisibleObject() override = default;

    /** Initialises the mesh's OpenGL data, only to be called by the render thread. */
    void initialise() override;

    /** Toggles whether the object should actually be rendered. */
    void hide();
    void show();

    void set_type( Type type );
    void set_shader( Shader * shader );

    void translate( glm::vec3 const & translation );
    void set_position( glm::vec3 const & position );

    void rotate( glm::vec3 const & axis, float angle );
    void set_rotation( glm::mat4 const & rotation );

    void scale( glm::vec3 const & scale );
    void scale( float scale );
    void set_scale( glm::vec3 const & scale );
    void set_scale( float scale );

    void update() override = 0;
    void draw() const override;

private:
    /// The main components for drawing this object.
    Type m_type;
    Mesh<ColourVertex> m_mesh;
    Shader * m_shader;

    /// The transformations defining where and how the object is located.
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::mat4 m_rotation;

    /// A toggle that can be used to skip this object during rendering.
    bool m_hidden { false };
};


#endif //DEMO_TD_RENDER_OBJECT_HPP
