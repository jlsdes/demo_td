#include "render_object.hpp"

#include "log.hpp"
#include "shader.hpp"

#include <glm/gtc/matrix_transform.hpp>


RenderObject::RenderObject( Type const type, Mesh && mesh, Shader * const shader )
    : m_type { type }, m_mesh { std::move( mesh ) }, m_shader { shader }, m_transform { glm::identity<glm::mat4>() } {
    if (shader == nullptr)
        Log::warning( "RenderObject was not provided with a Shader object." );
}

void RenderObject::draw() const {
    if ( m_hidden )
        return;
    m_shader->set_uniform( "model", m_transform );
    m_mesh.draw();
}

void RenderObject::hide() {
    m_hidden = true;
}

void RenderObject::show() {
    m_hidden = false;
}

void RenderObject::translate( glm::vec3 const & translation ) {
    m_transform = glm::translate( m_transform, translation );
}

void RenderObject::rotate( glm::vec3 const & axis, float const angle ) {
    m_transform = glm::rotate( m_transform, angle, axis );
}

void RenderObject::scale( glm::vec3 const & scale ) {
    m_transform = glm::scale( m_transform, scale );
}

void RenderObject::scale( float const scale ) {
    m_transform = glm::scale( m_transform, glm::vec3 { scale } );
}
