#include "view_object.hpp"
#include "shader.hpp"
#include "../utils/log.hpp"

#include <glm/gtc/matrix_transform.hpp>


ViewObject::ViewObject( Type const type, Mesh && mesh, Shader * const shader )
    : m_type { type }, m_mesh { std::move( mesh ) }, m_shader { shader }, m_transform { 1.f } {
    if (shader == nullptr)
        Log::warning( "RenderObject was not provided with a Shader object." );
}

void ViewObject::update() {}

void ViewObject::draw() const {
    if ( m_hidden )
        return;
    m_shader->set_uniform( "model", m_transform );
    m_mesh.draw();
}

void ViewObject::hide() {
    m_hidden = true;
}

void ViewObject::show() {
    m_hidden = false;
}

void ViewObject::translate( glm::vec3 const & translation ) {
    m_transform = glm::translate( m_transform, translation );
}

void ViewObject::rotate( glm::vec3 const & axis, float const angle ) {
    m_transform = glm::rotate( m_transform, angle, axis );
}

void ViewObject::scale( glm::vec3 const & scale ) {
    m_transform = glm::scale( m_transform, scale );
}

void ViewObject::scale( float const scale ) {
    m_transform = glm::scale( m_transform, glm::vec3 { scale } );
}
