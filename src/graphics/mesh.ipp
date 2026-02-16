#ifndef DEMO_TD_MESH_IPP
#define DEMO_TD_MESH_IPP

#include "utils/log.hpp"

#include <utility>


template <glm::length_t size, typename Number>
std::ostream & operator<<( std::ostream & stream, glm::vec<size, Number> const & vector ) {
    stream << '<' << vector[0];
    for ( unsigned int i { 1 }; i < size; ++i )
        stream << ", " << vector[i];
    return stream << '>';
}

template <VertexType V>
std::ostream & operator<<( std::ostream & stream, V const & vertex ) {
    stream << "<Vertex position=" << vertex.position;
    if constexpr ( V::has_normal )
        stream << " normal=" << vertex.normal();
    if constexpr ( V::has_colour )
        stream << " colour=" << vertex.colour();
    if constexpr ( V::has_texture )
        stream << " texture=" << vertex.texture();
    return stream << ">";
}

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

template <VertexType V>
void set_attribute( unsigned int & index, unsigned int & offset, int const size ) {
    glVertexAttribPointer( index, size, GL_FLOAT, GL_FALSE, sizeof( V ), reinterpret_cast<void *>(offset) );
    glEnableVertexAttribArray( index++ );
    offset += size * sizeof( float );
}

/** Helper function for the constructor; sets up the attribute pointers in OpenGL for each of the vertex attributes. */
template <VertexType V>
unsigned int set_vertex_attributes() {
    unsigned int index { 0 };
    unsigned int offset { 0 };

    set_attribute<V>( index, offset, 3 );
    if constexpr ( V::has_normal )
        set_attribute<V>( index, offset, 3 );
    if constexpr ( V::has_colour )
        set_attribute<V>( index, offset, 3 );
    if constexpr ( V::has_texture )
        set_attribute<V>( index, offset, 2 );

    return index;
}

template <VertexType V>
Mesh<V>::Mesh( std::vector<V> const & vertices, std::vector<unsigned int> const & indices, int const draw_mode )
    : m_vertices { vertices }, m_indices { indices }, m_vertex_buffer { 0 }, m_vertex_array { 0 },
      m_element_buffer { 0 }, m_draw_mode { draw_mode }, m_creation_thread { 0 }, m_flags { default_mesh_flags } {}

template <VertexType V>
Mesh<V>::~Mesh() {
    Mesh::destroy_gl_objects();
}

template <VertexType V>
Mesh<V>::Mesh( Mesh && mesh ) noexcept : m_vertices { std::move( mesh.m_vertices ) },
                                         m_indices { std::move( mesh.m_indices ) },
                                         m_vertex_buffer { std::exchange( mesh.m_vertex_buffer, 0 ) },
                                         m_vertex_array { std::exchange( mesh.m_vertex_array, 0 ) },
                                         m_element_buffer { std::exchange( mesh.m_element_buffer, 0 ) },
                                         m_draw_mode { std::exchange( mesh.m_draw_mode, GL_TRIANGLES ) },
                                         m_creation_thread { mesh.m_creation_thread },
                                         m_flags { std::exchange( mesh.m_flags, default_mesh_flags ) } {}

template <VertexType V>
Mesh<V> & Mesh<V>::operator=( Mesh && mesh ) noexcept {
    if ( &mesh == this )
        return *this;
    m_vertices = std::move( mesh.m_vertices );
    m_indices = std::move( mesh.m_indices );
    m_vertex_buffer = std::exchange( mesh.m_vertex_buffer, 0 );
    m_vertex_array = std::exchange( mesh.m_vertex_array, 0 );
    m_element_buffer = std::exchange( mesh.m_element_buffer, 0 );
    m_draw_mode = std::exchange( mesh.m_draw_mode, GL_TRIANGLES );
    m_creation_thread = mesh.m_creation_thread;
    m_flags = std::exchange( mesh.m_flags, default_mesh_flags );
    return *this;
}

template <VertexType V>
unsigned int Mesh<V>::initialise_gl_objects() {
    if ( get_flag( IsInitialised ) ) {
        Log::warning( "Attempted to initialise a Mesh twice, skipping second attempt." );
        return 0;
    }
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<V>( GL_ARRAY_BUFFER, m_vertices );
    if ( not m_indices.empty() ) {
        m_element_buffer = create_buffer<unsigned int>( GL_ELEMENT_ARRAY_BUFFER, m_indices );
        set_flag( HasIndex );
    }
    auto const index { set_vertex_attributes<V>() };

    // GL functions should only be called from the render thread, so creation and deletion of the buffers should happen
    // in the same thread.
    m_creation_thread = std::this_thread::get_id(); // Presumably the render thread
    set_flag( IsInitialised );

    return index;
}

template <VertexType V>
void Mesh<V>::destroy_gl_objects() {
    if ( get_flag( IsInitialised ) and std::this_thread::get_id() != m_creation_thread )
        // Issue an error, but attempt to destroy it anyway
        Log::warning( "Deleted GL data in a different thread to the one it was created in; may be leaking." );
    glDeleteVertexArrays( 1, &m_vertex_array ); // Silently ignores zeroes, so no existence check is required
    glDeleteBuffers( 1, &m_vertex_buffer );
    glDeleteBuffers( 1, &m_element_buffer );
    unset_flag( IsInitialised );
}

template <VertexType V>
void Mesh<V>::set_draw_mode( int const mode ) {
    assert( mode >= 0 && mode <= 6 );
    m_draw_mode = mode;
}

template <VertexType V>
void Mesh<V>::draw() const {
    glBindVertexArray( m_vertex_array );
    if ( get_flag( HasIndex ) )
        glDrawElements( m_draw_mode, static_cast<int>(m_indices.size()), GL_UNSIGNED_INT, nullptr );
    else
        glDrawArrays( m_draw_mode, 0, static_cast<int>(m_vertices.size()) );
}

template <VertexType V>
bool Mesh<V>::get_flag( MeshFlag const flag ) const {
    assert( flag < NumberMeshFlags );
    return m_flags[flag];
}

template <VertexType V>
void Mesh<V>::set_flag( MeshFlag const flag ) {
    assert( flag < NumberMeshFlags );
    m_flags[flag] = true;
}

template <VertexType V>
void Mesh<V>::unset_flag( MeshFlag const flag ) {
    assert( flag < NumberMeshFlags );
    m_flags[flag] = false;
}

template <VertexType V>
InstancedMesh<V>::InstancedMesh( std::vector<V> const & vertices,
                                 std::vector<unsigned int> const & indices,
                                 int const draw_mode )
    : Mesh<V> { vertices, indices, draw_mode }, m_instance_array {}, m_nr_instances { 0 }, m_instance_buffer { 0 } {
    Mesh<V>::set_flag( IsInstanced );
    Mesh<V>::set_flag( HasUpdated );
}

template <VertexType V>
InstancedMesh<V>::InstancedMesh( Mesh<V> && mesh ) noexcept
    : Mesh<V> { std::move( mesh ) }, m_instance_array {}, m_nr_instances { 0 }, m_instance_buffer { 0 } {
    Mesh<V>::set_flag( IsInstanced );
    Mesh<V>::set_flag( HasUpdated );
}

template <VertexType V>
InstancedMesh<V>::~InstancedMesh() {
    InstancedMesh::destroy_gl_objects();
}

template <VertexType V>
InstancedMesh<V>::InstancedMesh( InstancedMesh && mesh ) noexcept
    : Mesh<V> { mesh }, m_instance_array { std::move( mesh.m_instance_array ) },
      m_nr_instances { std::exchange( mesh.m_nr_instances, 0 ) },
      m_instance_buffer { std::exchange( mesh.m_instance_buffer, 0 ) } {}

template <VertexType V>
InstancedMesh<V> & InstancedMesh<V>::operator=( InstancedMesh && mesh ) noexcept {
    Mesh<V>::operator=( mesh );
    m_instance_array = std::move( mesh.m_instance_array );
    m_nr_instances = std::exchange( mesh.m_nr_instances, 0 );
    m_instance_buffer = std::exchange( mesh.m_instance_buffer, 0 );
    return *this;
}

template <VertexType V>
unsigned int InstancedMesh<V>::initialise_gl_objects() {
    unsigned int index { Mesh<V>::initialise_gl_objects() };
    unsigned int offset { 0 };

    glGenBuffers( 1, &m_instance_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, m_instance_buffer );
    glBufferData( GL_ARRAY_BUFFER, g_max_instances * sizeof( InstanceData ), nullptr, GL_DYNAMIC_DRAW );

    for ( unsigned int i { 0 }; i < 4; ++i ) {
        glEnableVertexAttribArray( index );
        glVertexAttribPointer( index, 4, GL_FLOAT, GL_FALSE, sizeof( InstanceData ), reinterpret_cast<void *>(offset) );
        glVertexAttribDivisor( index, 1 );
        ++index;
        offset += sizeof( glm::vec4 );
    }

    for ( unsigned int i { 0 }; i < 3; ++i ) {
        glEnableVertexAttribArray( index );
        glVertexAttribPointer( index, 3, GL_FLOAT, GL_FALSE, sizeof( InstanceData ), reinterpret_cast<void *>(offset) );
        glVertexAttribDivisor( index, 1 );
        ++index;
        offset += sizeof( glm::vec3 );
    }
    return index;
}

template <VertexType V>
void InstancedMesh<V>::destroy_gl_objects() {
    Mesh<V>::destroy_gl_objects();
    glDeleteBuffers( 1, &m_instance_buffer );
}

template <VertexType V>
unsigned int InstancedMesh<V>::get_nr_instances() const {
    return m_nr_instances;
}

template <VertexType V>
void InstancedMesh<V>::reset_data() {
    if ( m_nr_instances )
        Mesh<V>::set_flag( HasUpdated );
    m_nr_instances = 0;
}

template <VertexType V>
void InstancedMesh<V>::update( glm::mat4 const & transformation ) {
    glm::mat3 const normal_transformation { glm::mat3 { glm::transpose( glm::inverse( transformation ) ) } };
    m_instance_array[m_nr_instances] = { transformation, normal_transformation };
    ++m_nr_instances;
}

template <VertexType V>
void InstancedMesh<V>::draw() const {
    if ( Mesh<V>::get_flag( HasUpdated ) ) {
        glBindBuffer( GL_ARRAY_BUFFER, m_instance_buffer );
        glBufferSubData( GL_ARRAY_BUFFER, 0, m_nr_instances * sizeof( InstanceData ), m_instance_array.data() );
    }

    glBindVertexArray( Mesh<V>::m_vertex_array );
    if ( Mesh<V>::get_flag( HasIndex ) )
        glDrawElementsInstanced( Mesh<V>::m_draw_mode, static_cast<int>(Mesh<V>::m_indices.size()), GL_UNSIGNED_INT,
                                 nullptr, m_nr_instances );
    else
        glDrawArraysInstanced( Mesh<V>::m_draw_mode, 0, static_cast<int>(Mesh<V>::m_vertices.size()), m_nr_instances );
}


#endif //DEMO_TD_MESH_IPP
