#include "mesh.hpp"

#include "log.hpp"

#include <glad/gl.h>

#include <cassert>


std::ostream & operator<<( std::ostream & stream, glm::vec3 const & vector ) {
    return stream << '<' << vector.x << ", " << vector.y << ", " << vector.z << '>';
}

std::ostream & operator<<( std::ostream & stream, Vertex const & vertex ) {
    return stream << "<Vertex" << vertex.position << ", " << vertex.normal << ", " << vertex.colour << '>';
}

/// Helper function for the constructors; initialises an OpenGL buffer object, and copies data into it
template <typename ElementType>
unsigned int create_buffer( GLenum const buffer_type,
                            ElementType const * const data,
                            unsigned long const nr_elements ) {
    unsigned int buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( buffer_type, buffer );
    glBufferData( buffer_type, nr_elements * sizeof( ElementType ), data, GL_STATIC_DRAW );
    return buffer;
}

/// Helper function for the constructors; copies a vector's data into an array.
template <typename ElementType>
std::unique_ptr<ElementType[]> vector_to_array( std::vector<ElementType> const & data ) {
    auto array { std::make_unique<ElementType[]>( data.size() ) };
    std::ranges::copy( data.cbegin(), data.cend(), array.get() );
    return array;
}

/// Helper function for the constructor; sets up the attribute pointers in OpenGL for each of the vertex attributes.
void set_vertex_attributes() {
    // Set the position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );
    glEnableVertexAttribArray( 0 );
    // Set the normal vectors
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), reinterpret_cast<void *>(sizeof( glm::vec3 )) );
    glEnableVertexAttribArray( 1 );
    // Set the colour values
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<void *>(2 * sizeof( glm::vec3 )) );
    glEnableVertexAttribArray( 2 );
}

Mesh::Mesh( std::vector<Vertex> const & vertices, std::vector<unsigned int> const & indices, int const draw_mode )
    : m_vertices { vector_to_array( vertices ) }, m_nr_vertices { vertices.size() },
      m_indices { indices.empty() ? nullptr : vector_to_array( indices ) }, m_nr_indices { indices.size() },
      m_vertex_buffer { 0 }, m_vertex_array { 0 }, m_element_buffer { 0 }, m_default_mode { draw_mode } {
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<Vertex>( GL_ARRAY_BUFFER, m_vertices.get(), m_nr_vertices );
    if ( m_nr_indices > 0 )
        m_element_buffer = create_buffer<unsigned int>( GL_ELEMENT_ARRAY_BUFFER, m_indices.get(), m_nr_indices );
    set_vertex_attributes();
}

Mesh::~Mesh() {
    if ( m_vertex_array )
        glDeleteVertexArrays( 1, &m_vertex_array );
    if ( m_vertex_buffer )
        glDeleteBuffers( 1, &m_vertex_buffer );
    if ( m_element_buffer )
        glDeleteBuffers( 1, &m_element_buffer );
}

Mesh::Mesh( Mesh && mesh ) noexcept
    : m_vertices { std::move( mesh.m_vertices ) }, m_nr_vertices { mesh.m_nr_vertices },
      m_indices { mesh.m_indices ? std::move( mesh.m_indices ) : nullptr }, m_nr_indices { mesh.m_nr_indices },
      m_vertex_buffer { mesh.m_vertex_buffer }, m_vertex_array { mesh.m_vertex_array },
      m_element_buffer { mesh.m_element_buffer }, m_default_mode { mesh.m_default_mode } {
    mesh.m_vertex_buffer = 0;
    mesh.m_vertex_array = 0;
    mesh.m_element_buffer = 0;
}

bool Mesh::has_index() const {
    return m_element_buffer != 0;
}

void Mesh::set_draw_mode( int const mode ) {
    assert( mode >= 0 && mode <= 6 );
    m_default_mode = mode;
}

void Mesh::draw( int mode ) const {
    if ( mode == -1 )
        mode = m_default_mode;
    assert( mode >= 0 && mode <= 6 );

    glBindVertexArray( m_vertex_array );
    if ( has_index() )
        glDrawElements( mode, static_cast<int>(m_nr_indices * 3), GL_UNSIGNED_INT, nullptr );
    else
        glDrawArrays( mode, 0, static_cast<int>(m_nr_vertices) );
}
