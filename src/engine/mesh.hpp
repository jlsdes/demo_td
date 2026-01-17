#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>


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


template <glm::length_t size, typename T>
std::ostream & operator<<( std::ostream & stream, glm::vec<size, T> const & vector );

template <bool has_normal, bool has_colour, bool has_texture>
std::ostream & operator<<( std::ostream & stream, Vertex_<has_normal, has_colour, has_texture> const & vertex );


/** A mesh consisting of vertices and faces, optionally defined by vertex indices. This class holds some OpenGL objects,
 *  and must therefore always be created in the (main) render thread. */
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
    explicit Mesh( std::vector<ColourVertex> const & vertices,
                   std::vector<unsigned int> const & indices = {},
                   int draw_mode = GL_TRIANGLES );

    Mesh( Mesh const & mesh ) = delete;
    Mesh & operator=( Mesh const & mesh ) = delete;
    Mesh( Mesh && mesh ) noexcept;
    Mesh & operator=( Mesh && mesh ) noexcept;
    ~Mesh();

    /** Initialises the mesh's OpenGL data, only to be called by the main render thread. */
    void initialise();

    /** Returns whether the mesh has an index array configured. */
    [[nodiscard]] bool has_index() const;

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    void draw( int mode = -1 ) const;

private:
    /// The mesh data; could be used to modify the buffer data.
    std::vector<ColourVertex> m_vertices;
    std::vector<unsigned int> m_indices;

    /// OpenGL object IDs.
    unsigned int m_vertex_buffer;
    unsigned int m_vertex_array;
    unsigned int m_element_buffer;

    /// The current default mode for drawing this mesh.
    int m_default_mode;

    bool m_initialised;
};


// Template definitions

template <glm::length_t size, typename T>
std::ostream & operator<<( std::ostream & stream, glm::vec<size, T> const & vector ) {
    stream << '<' << vector[0];
    for ( unsigned int i { 1 }; i < size; ++i )
        stream << ", " << vector[i];
    return stream << '>';
}

template <bool has_normal, bool has_colour, bool has_texture>
std::ostream & operator<<( std::ostream & stream, Vertex_<has_normal, has_colour, has_texture> const & vertex ) {
    stream << "<Vertex position=" << vertex.position;
    if ( has_normal )
        stream << " normal=" << vertex.normal();
    if ( has_colour )
        stream << " colour=" << vertex.colour();
    if ( has_texture )
        stream << " texture=" << vertex.texture();
    return stream << ">";
}


#endif //DEMO_TD_MESH_HPP
