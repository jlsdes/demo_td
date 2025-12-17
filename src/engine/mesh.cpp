#include "mesh.hpp"

#include <glad/gl.h>

#include <cassert>


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

template <typename ElementType>
std::unique_ptr<ElementType[]> vector_to_array( std::vector<ElementType> const & data ) {
    auto array { std::make_unique<ElementType[]>( data.size() ) };
    std::ranges::copy( data.cbegin(), data.cend(), array.get() );
    return array;
}

Mesh::Mesh( std::vector<Vertex> const & vertices, int const draw_mode )
    : m_vertices { vector_to_array( vertices ) }, m_nr_vertices { vertices.size() }, m_indices { nullptr },
      m_nr_faces { 0 }, m_vertex_buffer { 0 }, m_vertex_array { 0 }, m_element_buffer { 0 },
      m_default_mode { draw_mode } {
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<Vertex>( GL_ARRAY_BUFFER, m_vertices.get(), m_nr_vertices );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr );
    glEnableVertexAttribArray( 0 );
}

Mesh::Mesh( std::vector<Vertex> const & vertices, std::vector<unsigned int> const & faces, int const draw_mode )
    : m_vertices { vector_to_array( vertices ) }, m_nr_vertices { vertices.size() },
      m_indices { vector_to_array( faces ) }, m_nr_faces { faces.size() }, m_vertex_buffer { 0 }, m_vertex_array { 0 },
      m_element_buffer { 0 }, m_default_mode { draw_mode } {
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<Vertex>( GL_ARRAY_BUFFER, m_vertices.get(), m_nr_vertices );
    m_element_buffer = create_buffer<unsigned int>( GL_ELEMENT_ARRAY_BUFFER, m_indices.get(), m_nr_faces );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr );
    glEnableVertexAttribArray( 0 );
}

Mesh::~Mesh() {
    glDeleteVertexArrays( 1, &m_vertex_array );
    glDeleteBuffers( 1, &m_vertex_buffer );
    if ( has_index() )
        glDeleteBuffers( 1, &m_element_buffer );
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
        glDrawElements( mode, static_cast<int>(m_nr_faces * 3), GL_UNSIGNED_INT, nullptr );
    else
        glDrawArrays( mode, 0, static_cast<int>(m_nr_vertices) );
}
