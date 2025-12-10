#include "mesh.hpp"

#include <glad/gl.h>

#include <cassert>


Mesh::Mesh( std::vector<float> const & vertices,
            int const draw_mode )
    : m_vertex_buffer { 0 }, m_vertex_array { 0 }, m_element_buffer { 0 }, m_nr_drawn_vertices { vertices.size() },
      m_default_mode { draw_mode }
{
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<float>( GL_ARRAY_BUFFER, vertices );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr );
    glEnableVertexAttribArray( 0 );
}

Mesh::Mesh( std::vector<float> const & vertices,
            std::vector<unsigned int> const & indices,
            int const draw_mode )
    : m_vertex_buffer { 0 }, m_vertex_array { 0 }, m_element_buffer { 0 }, m_nr_drawn_vertices { indices.size() },
      m_default_mode { draw_mode }
{
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<float>( GL_ARRAY_BUFFER, vertices );
    m_element_buffer = create_buffer<unsigned int>( GL_ELEMENT_ARRAY_BUFFER, indices );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr );
    glEnableVertexAttribArray( 0 );
}

Mesh::~Mesh()
{
    glDeleteVertexArrays( 1, &m_vertex_array );
    glDeleteBuffers( 1, &m_vertex_buffer );
    if ( has_index() )
        glDeleteBuffers( 1, &m_element_buffer );
}

bool Mesh::has_index() const
{
    return m_element_buffer != 0;
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
    if ( has_index() )
        glDrawElements( mode, static_cast<int>(m_nr_drawn_vertices), GL_UNSIGNED_INT, nullptr );
    else
        glDrawArrays( mode, 0, static_cast<int>(m_nr_drawn_vertices) );
}
