#include "view_object.hpp"
#include "shader.hpp"
#include "utils/log.hpp"

#include <glm/gtc/matrix_transform.hpp>


VisibleObject::VisibleObject( ModelObject * const model,
                                Type const type,
                                Mesh<ColourVertex> && mesh,
                                Shader * const shader )
    : ViewObject { model }, m_type { type }, m_mesh { std::move( mesh ) }, m_shader { shader }, m_position { 0.f },
      m_scale { 1.f }, m_rotation { glm::identity<glm::mat4>() } {
    if ( not m_model )
        Log::warning( "Created a ViewableObject without a ModelObject attached." );
    if ( shader == nullptr )
        Log::warning( "Created a ViewableObject without providing a Shader." );
}

ViewObject::ViewObject( ModelObject * const model ) : m_model { model } {}

void ViewObject::initialise() {}

void VisibleObject::hide() {
    m_hidden = true;
}

void VisibleObject::show() {
    m_hidden = false;
}

void VisibleObject::translate( glm::vec3 const & translation ) {
    m_position += translation;
}

void VisibleObject::set_position( glm::vec3 const & position ) {
    m_position = position;
}

void VisibleObject::rotate( glm::vec3 const & axis, float const angle ) {
    m_rotation = glm::rotate( m_rotation, angle, axis );
}

void VisibleObject::set_rotation( glm::mat4 const & rotation ) {
    m_rotation = rotation;
}

void VisibleObject::scale( glm::vec3 const & scale ) {
    m_scale *= scale;
}

void VisibleObject::scale( float const scale ) {
    m_scale *= scale;
}

void VisibleObject::set_scale( glm::vec3 const & scale ) {
    m_scale = scale;
}

void VisibleObject::set_scale( float const scale ) {
    m_scale = glm::vec3 { scale };
}

void VisibleObject::initialise() {
    m_mesh.initialise_gl_objects();
}

void VisibleObject::draw() const {
    if ( m_hidden )
        return;
    // TODO check this, I doubt this is correct
    auto const transform { glm::scale( glm::translate( m_rotation, m_position ), m_scale ) };
    m_shader->set_uniform( "model", transform );
    m_mesh.draw();
}
