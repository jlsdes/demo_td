#include "mesh.hpp"

#include <glad/gl.h>

#include <cassert>


Mesh::Mesh( float const * const vertices, unsigned int const nr_vertices, int const draw_mode )
    : m_vertex_buffer {}, m_vertex_array {}, m_nr_vertices { nr_vertices }, m_default_mode { draw_mode }
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

void Mesh::set_draw_mode( int const mode )
{
    assert( mode >= 0 && mode <= 6 );
    m_default_mode = mode;
}


void Mesh::draw( int mode ) const
{
    if ( mode == -1 )
        mode = m_default_mode;
    assert( mode >= 0 && mode <= 6 );

    glBindVertexArray( m_vertex_array );
    glDrawArrays( mode, 0, static_cast<int>(m_nr_vertices) );
}
