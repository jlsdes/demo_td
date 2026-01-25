#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include "utils/log.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <memory>
#include <thread>
#include <vector>
#include <utility>


/** A variable that may not exist, primarily as a utility struct for the Vertex_ implementation. If 'exists' is true,
 *  then this struct has a single member 'value' with type 'T', but if it's false then this struct is empty. This can be
 *  useful if [[no_unique_address]] is used, as the compiler can then hide the Optional<...> member within another data
 *  member of the Vertex_ struct. 'Tag' is only here because it can force instances to be of different types, which is a
 *  requirement for [[no_unique_address]] to fully work. */
template <typename T, bool exists_, unsigned int Tag>
struct Optional {
    T value;

    T & operator*() { return value; }
    T * operator->() { return &value; }

    static constexpr bool exists { exists_ };
};

template <typename T, unsigned int Tag>
struct Optional<T, false, Tag> {};

/** A modular struct with some optional data fields. Depending on the template parameters some of the members can be
 *  empty. For example, if 'has_colour' is false, then 'colour' is an empty struct and 'colour.value' does not exist. */
template <bool has_normal_, bool has_colour_, bool has_texture_>
struct Vertex_ {
    glm::vec3 position;
    [[no_unique_address]] Optional<glm::vec3, has_normal_, 0> normal;
    [[no_unique_address]] Optional<glm::vec3, has_colour_, 1> colour;
    [[no_unique_address]] Optional<glm::vec2, has_texture_, 2> texture;

    static constexpr bool has_normal { has_normal_ };
    static constexpr bool has_colour { has_colour_ };
    static constexpr bool has_texture { has_texture_ };
};

using LineVertex = Vertex_<false, true, false>;
using ColourVertex = Vertex_<true, true, false>;
using TextureVertex = Vertex_<true, false, true>;
using Vertex = Vertex_<true, true, true>;

/// Requires a type to be an instantiation of the Vertex_ template.
template <class T>
concept VertexType = requires( T v ) { { Vertex_ { v } } -> std::same_as<T>; };


template <glm::length_t size, typename Number>
std::ostream & operator<<( std::ostream & stream, glm::vec<size, Number> const & vector );

template <VertexType V>
std::ostream & operator<<( std::ostream & stream, V const & vertex );


/** A mesh consisting of vertices and faces, optionally defined by vertex indices. This class holds some OpenGL objects,
 *  and must therefore always be created in the (main) render thread. */
template <VertexType V>
class Mesh {
public:
    /** Constructor; creates some underlying OpenGL buffers.
     *
     * @param vertices The vertices of the mesh.
     * @param indices The indices used to define the faces of the mesh, depending on the draw_mode. If this parameter is
     *  omitted, then a sequence {0, 1, 2...} is used by default.
     * @param draw_mode The default draw mode for the mesh as used in OpenGL. The default value 'GL_TRIANGLES' means
     *  that groups of three vertices will be combined to define some triangles.
     */
    explicit Mesh( std::vector<V> const & vertices,
                   std::vector<unsigned int> const & indices = {},
                   int draw_mode = GL_TRIANGLES );

    Mesh( Mesh const & mesh ) = delete;
    Mesh & operator=( Mesh const & mesh ) = delete;
    Mesh( Mesh && mesh ) noexcept;
    Mesh & operator=( Mesh && mesh ) noexcept;
    ~Mesh();

    /** Initialises the mesh's OpenGL data, only to be called by the main render thread. */
    void initialise_gl_objects();
    void destroy_gl_objects();

    /** Returns whether the mesh has an index array configured. */
    [[nodiscard]] bool has_index() const;

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    void draw( int mode = -1 ) const;

private:
    /// The mesh data; could be used to modify the buffer data.
    std::vector<V> m_vertices;
    std::vector<unsigned int> m_indices;

    /// OpenGL object IDs.
    unsigned int m_vertex_buffer;
    unsigned int m_vertex_array;
    unsigned int m_element_buffer;

    /// The current default mode for drawing this mesh.
    int m_default_mode;

    bool m_initialised;
    std::thread::id m_creation_thread;
};


// Template definitions

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
void set_vertex_attributes() {
    unsigned int index { 0 };
    unsigned int offset { 0 };

    set_attribute<V>( index, offset, 3 );
    if constexpr ( V::has_normal )
        set_attribute<V>( index, offset, 3 );
    if constexpr ( V::has_colour )
        set_attribute<V>( index, offset, 3 );
    if constexpr ( V::has_normal )
        set_attribute<V>( index, offset, 2 );
}

template <VertexType V>
Mesh<V>::Mesh( std::vector<V> const & vertices, std::vector<unsigned int> const & indices, int const draw_mode )
    : m_vertices { vertices }, m_indices { indices }, m_vertex_buffer { 0 }, m_vertex_array { 0 },
      m_element_buffer { 0 }, m_default_mode { draw_mode }, m_initialised { false }, m_creation_thread { 0 } {}

template <VertexType V>
Mesh<V>::Mesh( Mesh && mesh ) noexcept : m_vertices { std::move( mesh.m_vertices ) },
                                      m_indices { std::move( mesh.m_indices ) },
                                      m_vertex_buffer { std::exchange( mesh.m_vertex_buffer, 0 ) },
                                      m_vertex_array { std::exchange( mesh.m_vertex_array, 0 ) },
                                      m_element_buffer { std::exchange( mesh.m_element_buffer, 0 ) },
                                      m_default_mode { std::exchange( mesh.m_default_mode, GL_TRIANGLES ) },
                                      m_initialised { std::exchange( mesh.m_initialised, false ) },
                                      m_creation_thread { mesh.m_creation_thread } {}

template <VertexType V>
Mesh<V> & Mesh<V>::operator=( Mesh && mesh ) noexcept {
    if ( &mesh == this )
        return *this;
    m_vertices = std::move( mesh.m_vertices );
    m_indices = std::move( mesh.m_indices );
    m_vertex_buffer = std::exchange( mesh.m_vertex_buffer, 0 );
    m_vertex_array = std::exchange( mesh.m_vertex_array, 0 );
    m_element_buffer = std::exchange( mesh.m_element_buffer, 0 );
    m_default_mode = std::exchange( mesh.m_default_mode, GL_TRIANGLES );
    m_initialised = std::exchange( mesh.m_initialised, false );
    m_creation_thread = mesh.m_creation_thread;
    return *this;
}

template <VertexType V>
Mesh<V>::~Mesh() {
    destroy_gl_objects();
}

template <VertexType V>
void Mesh<V>::initialise_gl_objects() {
    if ( m_initialised ) {
        Log::warning( "Attempted to initialise a Mesh twice, skipping second attempt." );
        return;
    }
    glGenVertexArrays( 1, &m_vertex_array );
    glBindVertexArray( m_vertex_array );
    m_vertex_buffer = create_buffer<ColourVertex>( GL_ARRAY_BUFFER, m_vertices );
    if ( not m_indices.empty() )
        m_element_buffer = create_buffer<unsigned int>( GL_ELEMENT_ARRAY_BUFFER, m_indices );
    set_vertex_attributes<ColourVertex>();

    // GL functions should only be called from the render thread, so creation and deletion of the buffers should happen
    // in the same thread.
    m_creation_thread = std::this_thread::get_id(); // Presumably the render thread
    m_initialised = true;
}

template <VertexType V>
void Mesh<V>::destroy_gl_objects() {
    if ( m_initialised and std::this_thread::get_id() != m_creation_thread )
        // Issue an error, but attempt to destroy it anyway
        Log::warning( "Deleted GL data in a different thread to the one it was created in; may be leaking." );
    glDeleteVertexArrays( 1, &m_vertex_array ); // Silently ignores zeroes, so no existence check is required
    glDeleteBuffers( 1, &m_vertex_buffer );
    glDeleteBuffers( 1, &m_element_buffer );
    m_initialised = false;
}

template <VertexType V>
bool Mesh<V>::has_index() const {
    return not m_indices.empty();
}

template <VertexType V>
void Mesh<V>::set_draw_mode( int const mode ) {
    assert( mode >= 0 && mode <= 6 );
    m_default_mode = mode;
}

template <VertexType V>
void Mesh<V>::draw( int mode ) const {
    if ( mode == -1 )
        mode = m_default_mode;
    assert( mode >= 0 && mode <= 6 );

    glBindVertexArray( m_vertex_array );
    if ( has_index() )
        glDrawElements( mode, static_cast<int>(m_indices.size()), GL_UNSIGNED_INT, nullptr );
    else
        glDrawArrays( mode, 0, static_cast<int>(m_vertices.size()) );
}

#endif //DEMO_TD_MESH_HPP
