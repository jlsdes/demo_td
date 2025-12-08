#include "mesh.hpp"

#include <glad/gl.h>


Mesh::Mesh( float const * const vertices, unsigned int const nr_vertices )
    : m_vertex_buffer {}, m_vertex_array {}, m_nr_vertices {nr_vertices}
{
    // Initialise the vertex buffer
    glGenBuffers( 1, &m_vertex_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, m_vertex_buffer );
    glBufferData( GL_ARRAY_BUFFER, static_cast<long>(3 * nr_vertices * sizeof( float )), vertices, GL_STATIC_DRAW );

    // Initialise the vertex array object
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
}

Mesh::~Mesh()
{
    glDeleteVertexArrays( 1, &m_vertex_array );
    glDeleteBuffers( 1, &m_vertex_buffer );
}

void Mesh::draw() const
{
    glBindVertexArray( m_vertex_array );
    glDrawArrays( GL_TRIANGLES, 0, static_cast<int>( m_nr_vertices ) );
}
