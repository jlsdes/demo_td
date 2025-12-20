#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <memory>
#include <ostream>
#include <vector>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 colour;
    // TODO add texture coordinates? Although I might not use them in this project
};

std::ostream & operator<<( std::ostream & stream, glm::vec3 const & vector );
std::ostream & operator<<( std::ostream & stream, Vertex const & vertex );


/** A basic mesh. */
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
    explicit Mesh( std::vector<Vertex> const & vertices,
                   std::vector<unsigned int> const & indices = {},
                   int draw_mode = GL_TRIANGLES );

    /** Destructor; deletes the underlying OpenGL buffer objects. */
    ~Mesh();

    /** Move constructor. */
    Mesh( Mesh && mesh ) noexcept;

    /** Returns whether the mesh has an index array configured. */
    [[nodiscard]] bool has_index() const;

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    void draw( int mode = -1 ) const;

private:
    /// The mesh data; could be used to modify the buffer data.
    std::unique_ptr<Vertex[]> m_vertices;
    unsigned long m_nr_vertices;
    std::unique_ptr<unsigned int[]> m_indices;
    unsigned long m_nr_indices;

    /// OpenGL object IDs.
    unsigned int m_vertex_buffer;
    unsigned int m_vertex_array;
    unsigned int m_element_buffer;

    /// The current default mode for drawing this mesh.
    int m_default_mode;
};


#endif //DEMO_TD_MESH_HPP
