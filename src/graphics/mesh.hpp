#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP


/** A basic mesh. */
class Mesh
{
public:
    /** Constructor. */
    Mesh( float const * vertices, unsigned int nr_vertices );

    /** Destructor. */
    ~Mesh();

    /** Draws the mesh. */
    void draw() const;

private:
    /// Vertex buffer object.
    unsigned int m_vertex_buffer;
    /// Vertex array object.
    unsigned int m_vertex_array;
    /// The number of vertices in the mesh.
    unsigned int m_nr_vertices;
};


#endif //DEMO_TD_MESH_HPP
