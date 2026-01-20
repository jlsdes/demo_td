#include "view_object.hpp"
#include "shader.hpp"
#include "utils/log.hpp"

#include <glm/gtc/matrix_transform.hpp>


ViewObject::ViewObject( ModelObject * const model, Type const type, Mesh<ColourVertex> && mesh, Shader * const shader )
    : m_model { model }, m_type { type }, m_mesh { std::move( mesh ) }, m_shader { shader }, m_position { 0.f },
      m_scale { 1.f }, m_rotation { glm::identity<glm::mat4>() } {
    if ( not m_model )
        Log::warning( "Created a ViewObject without a ModelObject attached." );
    if ( shader == nullptr )
        Log::warning( "Created a ViewObject without providing a Shader." );
}

void ViewObject::initialise_mesh() {
    m_mesh.initialise_gl_objects();
}

void ViewObject::update() {}

void ViewObject::draw() const {
    if ( m_hidden )
        return;
    // TODO check this, I doubt this is correct
    auto const transform { glm::scale( glm::translate( m_rotation, m_position ), m_scale ) };
    m_shader->set_uniform( "model", transform );
    m_mesh.draw();
}

void ViewObject::hide() {
    m_hidden = true;
}

void ViewObject::show() {
    m_hidden = false;
}

void ViewObject::translate( glm::vec3 const & translation ) {
    m_position += translation;
}

void ViewObject::set_position( glm::vec3 const & position ) {
    m_position = position;
}

void ViewObject::rotate( glm::vec3 const & axis, float const angle ) {
    m_rotation = glm::rotate( m_rotation, angle, axis );
}

void ViewObject::set_rotation( glm::mat4 const & rotation ) {
    m_rotation = rotation;
}

void ViewObject::scale( glm::vec3 const & scale ) {
    m_scale *= scale;
}

void ViewObject::scale( float const scale ) {
    m_scale *= scale;
}

void ViewObject::set_scale( glm::vec3 const & scale ) {
    m_scale = scale;
}

void ViewObject::set_scale( float const scale ) {
    m_scale = glm::vec3 { scale };
}
