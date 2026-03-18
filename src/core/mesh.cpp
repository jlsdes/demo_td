#include "mesh.hpp"

#include <cassert>
#include <thread>
#include <vector>

#include <glad/gl.h>


Mesh::Mesh( std::vector<Vertex> const & vertices, std::vector<unsigned int>const & indices )
        : m_vao { 0 }, m_vbo { 0 }, m_ebo { 0 }, m_count { vertices.size() },
          m_home_thread { std::this_thread::get_id() } {
    glGenVertexArrays( 1, &m_vao );
    glGenBuffers( 1, &m_vbo );
    if ( not indices.empty() )
        glGenBuffers( 1, &m_ebo );

    glVertexArrayVertexBuffer( m_vao, 0, m_vbo, 0, sizeof( Vertex ) );
    glNamedBufferData( m_vbo, vertices.size() * sizeof( Vertex ), vertices.data(), GL_STATIC_DRAW );

    glEnableVertexArrayAttrib( m_vao, 0 );
    glVertexArrayAttribFormat( m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, position ) );
    glVertexArrayAttribBinding( m_vao, 0, 0 );

    glEnableVertexArrayAttrib( m_vao, 1 );
    glVertexArrayAttribFormat( m_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, normal ) );
    glVertexArrayAttribBinding( m_vao, 1, 0 );

    glEnableVertexArrayAttrib( m_vao, 2 );
    glVertexArrayAttribFormat( m_vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof( Vertex, colour ) );
    glVertexArrayAttribBinding( m_vao, 2, 0 );

    if ( not indices.empty() ) {
        glVertexArrayElementBuffer( m_vao, m_ebo );
        glNamedBufferData( m_ebo, indices.size() * sizeof( unsigned int ), indices.data(), GL_STATIC_DRAW );

        m_count = indices.size();
    }
}

Mesh::~Mesh() {
    assert( std::this_thread::get_id() == m_home_thread );

    if ( m_ebo )
        glDeleteBuffers( 1, &m_ebo );
    glDeleteBuffers( 1, &m_vbo );
    glDeleteVertexArrays( 1, &m_vao );
}

void Mesh::draw() {
    assert( std::this_thread::get_id() == m_home_thread );

    glDrawArrays( GL_TRIANGLES, 0, m_count );
}
