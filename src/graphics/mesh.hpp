#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include <glad/gl.h>


/** A basic mesh. */
class Mesh
{
public:
    /** Constructor. */
    Mesh( float const * vertices, unsigned int nr_vertices, int draw_mode = GL_TRIANGLES );

    /** Destructor. */
    ~Mesh();

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    void draw( int mode = -1 ) const;

private:
    /// Vertex buffer object.
    unsigned int m_vertex_buffer;
    /// Vertex array object.
    unsigned int m_vertex_array;

    /// The number of vertices in the mesh.
    unsigned int m_nr_vertices;

    /// The current default mode for drawing this mesh.
    int m_default_mode;
};


#endif //DEMO_TD_MESH_HPP
