#include "mesh.hpp"
#include "utils/log.hpp"

#include <glad/gl.h>

#include <cassert>
#include <utility>


/** Creates a new OpenGL buffer and copies data into it. */
template <typename ElementType>
unsigned int create_buffer( GLenum const buffer_type, std::vector<ElementType> const & data ) {
    unsigned int buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( buffer_type, buffer );
    // Creating the buffer with 'nullptr' as its data argument will only allocate memory, and not assign anything
    glBufferData( buffer_type, data.size() * sizeof( ElementType ), nullptr, GL_STATIC_DRAW );

    void * buffer_data { glMapBuffer( buffer_type, GL_WRITE_ONLY ) };
    std::ranges::copy( data.cbegin(), data.cend(), static_cast<ElementType *>(buffer_data) );
    glUnmapBuffer( buffer_type );

    return buffer;
}

template <class T>
concept VertexType = requires( T v ) { { Vertex_ { v } } -> std::same_as<T>; };

template <VertexType T>
void set_attribute( unsigned int & index, unsigned int & offset, int const size ) {
    glVertexAttribPointer( index, size, GL_FLOAT, GL_FALSE, sizeof( T ), reinterpret_cast<void *>(offset) );
    glEnableVertexAttribArray( index++ );
    offset += size * sizeof( float );
}

/** Helper function for the constructor; sets up the attribute pointers in OpenGL for each of the vertex attributes. */
template <VertexType T>
void set_vertex_attributes() {
    unsigned int index { 0 };
    unsigned int offset { 0 };

    set_attribute<T>( index, offset, 3 );
    if constexpr ( T::has_normal )
        set_attribute<T>( index, offset, 3 );
    if constexpr ( T::has_colour )
        set_attribute<T>( index, offset, 3 );
    if constexpr ( T::has_normal )
        set_attribute<T>( index, offset, 2 );
}

Mesh::Mesh( std::vector<ColourVertex> const & vertices, std::vector<unsigned int> const & indices, int const draw_mode )
    : m_vertices { vertices }, m_indices { indices }, m_vertex_buffer { 0 }, m_vertex_array { 0 },
      m_element_buffer { 0 }, m_default_mode { draw_mode }, m_initialised { false } {}

Mesh::Mesh( Mesh && mesh ) noexcept : m_vertices { std::move( mesh.m_vertices ) },
                                      m_indices { std::move( mesh.m_indices ) },
                                      m_vertex_buffer { std::exchange( mesh.m_vertex_buffer, 0 ) },
                                      m_vertex_array { std::exchange( mesh.m_vertex_array, 0 ) },
                                      m_element_buffer { std::exchange( mesh.m_element_buffer, 0 ) },
                                      m_default_mode { std::exchange( mesh.m_default_mode, GL_TRIANGLES ) },
                                      m_initialised { std::exchange( mesh.m_initialised, false ) } {}

Mesh & Mesh::operator=( Mesh && mesh ) noexcept {
    if ( &mesh == this )
        return *this;
    m_vertices = std::move( mesh.m_vertices );
    m_indices = std::move( mesh.m_indices );
    m_vertex_buffer = std::exchange( mesh.m_vertex_buffer, 0 );
    m_vertex_array = std::exchange( mesh.m_vertex_array, 0 );
    m_element_buffer = std::exchange( mesh.m_element_buffer, 0 );
    m_default_mode = std::exchange( mesh.m_default_mode, GL_TRIANGLES );
    m_initialised = std::exchange( mesh.m_initialised, false );
    return *this;
}

Mesh::~Mesh() {
    if ( m_vertex_array )
        glDeleteVertexArrays( 1, &m_vertex_array );
    if ( m_vertex_buffer )
        glDeleteBuffers( 1, &m_vertex_buffer );
    if ( m_element_buffer )
        glDeleteBuffers( 1, &m_element_buffer );
}

void Mesh::initialise() {
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<ColourVertex>( GL_ARRAY_BUFFER, m_vertices );
    if ( not m_indices.empty() )
        m_element_buffer = create_buffer<unsigned int>( GL_ELEMENT_ARRAY_BUFFER, m_indices );
    set_vertex_attributes<ColourVertex>();
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
        glDrawElements( mode, static_cast<int>(m_indices.size() * 3), GL_UNSIGNED_INT, nullptr );
    else
        glDrawArrays( mode, 0, static_cast<int>(m_vertices.size()) );
}
