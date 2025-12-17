#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include <glad/gl.h>

#include <memory>
#include <vector>


struct Vector3 {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

std::ostream & operator<<( std::ostream & stream, Vector3 const & vector );

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector3 colour;
    // TODO add texture coordinates? Although I might not use them in this project
};

std::ostream & operator<<( std::ostream & stream, Vertex const & vertex );


/** A basic mesh. */
class Mesh {
public:
    /** Constructor without vertex indices.
     *
     * @param vertices The vertex positions of the mesh. Each group of 3 vertices will be used to create 1 face.
     * @param draw_mode The default draw mode for the mesh.
     */
    explicit Mesh( std::vector<Vertex> const & vertices, int draw_mode = GL_TRIANGLES );

    /** Constructor with vertex indices.
     *
     * @param vertices The coordinates of the vertices of the mesh.
     * @param indices The faces of the mesh, each face is defined by the indices of its vertices.
     * @param draw_mode The default draw mode for the mesh.
     */
    Mesh( std::vector<Vertex> const & vertices, std::vector<unsigned int> const & indices, int draw_mode = GL_TRIANGLES );

    /** Destructor. */
    ~Mesh();

    /** Returns whether the mesh has an index array configured. */
    [[nodiscard]] bool has_index() const;

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    void draw( int mode = -1 ) const;

private:
    /// The mesh data.
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
