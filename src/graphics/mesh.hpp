#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include "entity/entity.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <bitset>
#include <thread>
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

/// Requires a type to be an instantiation of the Vertex_ template.
template <class T>
concept VertexType = requires( T v ) { { Vertex_ { v } } -> std::same_as<T>; };


template <glm::length_t size, typename Number>
std::ostream & operator<<( std::ostream & stream, glm::vec<size, Number> const & vector );

template <VertexType V>
std::ostream & operator<<( std::ostream & stream, V const & vertex );


enum MeshFlag : unsigned char {
    IsInitialised,
    IsHidden,
    IsLightSource,
    IsInstanced,
    HasIndex,
    HasUpdated,
    NumberMeshFlags // Must be the last enum value; indicates the number of flags but is not a valid flag index itself
};

constexpr std::bitset<NumberMeshFlags> default_mesh_flags { 0x00 };


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

    virtual ~Mesh();

    Mesh( Mesh const & ) = delete;
    Mesh & operator=( Mesh const & ) = delete;

    Mesh( Mesh && mesh ) noexcept;
    Mesh & operator=( Mesh && mesh ) noexcept;

    /** Initialises the mesh's OpenGL data, only to be called by the main render thread. */
    virtual unsigned int initialise_gl_objects();
    virtual void destroy_gl_objects();

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    virtual void draw() const;

    [[nodiscard]] bool get_flag( MeshFlag flag ) const;
    void set_flag( MeshFlag flag );
    void unset_flag( MeshFlag flag );

protected: // Using protected to allow InstancedMesh<> to draw everything itself without a bunch of accessor functions
    /// The mesh data; could be used to modify the buffer data.
    std::vector<V> m_vertices;
    std::vector<unsigned int> m_indices;

    /// OpenGL object IDs.
    unsigned int m_vertex_buffer;
    unsigned int m_vertex_array;
    unsigned int m_element_buffer;

    /// The current default mode for drawing this mesh.
    int m_draw_mode;

    std::thread::id m_creation_thread;

    std::bitset<NumberMeshFlags> m_flags;
};


unsigned int constexpr g_max_instances { g_max_entities };


/** A mesh class that can draw multiple instances of itself with just a single draw call. Every call to update() will
 *  add a single instance to be drawn on the next draw() call, after which the list should be cleared again. */
template <VertexType V>
class InstancedMesh : public Mesh<V> {
public:
    /** Constructor; creates some underlying OpenGL buffers.
     *
     * @param vertices The vertices of the mesh.
     * @param indices The indices used to define the faces of the mesh, depending on the draw_mode. If this parameter is
     *  omitted, then a sequence {0, 1, 2...} is used by default.
     * @param draw_mode The default draw mode for the mesh as used in OpenGL. The default value 'GL_TRIANGLES' means
     *  that groups of three vertices will be combined to define some triangles.
     */
    explicit InstancedMesh( std::vector<V> const & vertices,
                            std::vector<unsigned int> const & indices = {},
                            int draw_mode = GL_TRIANGLES );
    explicit InstancedMesh( Mesh<V> && mesh ) noexcept;

    ~InstancedMesh() override;

    InstancedMesh( InstancedMesh const & ) = delete;
    InstancedMesh & operator=( InstancedMesh const & ) = delete;

    InstancedMesh( InstancedMesh && mesh ) noexcept;
    InstancedMesh & operator=( InstancedMesh && mesh ) noexcept;

    /** Initialises the mesh's OpenGL data, only to be called by the main render thread. */
    unsigned int initialise_gl_objects() override;
    void destroy_gl_objects() override;

    [[nodiscard]] unsigned int get_nr_instances() const;

    /** Updates a single instance's transformation matrix. */
    void reset_data();
    void update( glm::mat4 const & transformation );

    void draw() const override;

private:
    struct InstanceData {
        glm::mat4 transform;
        glm::mat3 normal_transform;
    };

    std::array<InstanceData, g_max_instances> m_instance_array;
    unsigned int m_nr_instances;

    /// OpenGL object ID.
    unsigned int m_instance_buffer;
};


// Template definitions
#include "mesh.ipp"


#endif //DEMO_TD_MESH_HPP
