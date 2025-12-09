#ifndef DEMO_TD_MESH_HPP
#define DEMO_TD_MESH_HPP

#include <glad/gl.h>

#include <memory>
#include <vector>


/** A basic mesh. */
class Mesh
{
public:
    /** Constructor without vertex indices.
     *
     * @param vertices The vertex positions of the mesh. The order of the vertices in this argument also determines how
     *  the mesh will be drawn.
     * @param draw_mode The default draw mode for the mesh.
     */
    explicit Mesh( std::vector<float> const & vertices,
                   int draw_mode = GL_TRIANGLES );

    /** Constructor with vertex indices.
     *
     * @param vertices The coordinates of the vertices of the mesh.
     * @param indices The indices of the vertices used when drawing the mesh.
     * @param draw_mode The default draw mode for the mesh.
     */
    Mesh( std::vector<float> const & vertices,
          std::vector<unsigned int> const & indices,
          int draw_mode = GL_TRIANGLES );

    /** Destructor. */
    ~Mesh();

    /** Returns whether the mesh has an index array configured. */
    [[nodiscard]] bool has_index() const;

    /** Adds an index to the mesh. */
    void set_index( unsigned int const * indices,
                    unsigned int nr_indices );

    /** Sets a new default mode for drawing the mesh. */
    void set_draw_mode( int mode );

    /** Draws the mesh using the given draw mode, or its default draw mode if none is given. The default draw mode for
     * this mesh can be changed using set_draw_mode(). */
    void draw( int mode = -1 ) const;

private:
    /// OpenGL object IDs.
    unsigned int m_vertex_buffer;
    unsigned int m_vertex_array;
    unsigned int m_element_buffer;

    /// The vertex data.
    std::unique_ptr<float[]> m_vertices;
    unsigned long m_nr_vertices;
    std::unique_ptr<unsigned int[]> m_indices;
    unsigned long m_nr_indices;

    /// The current default mode for drawing this mesh.
    int m_default_mode;

    /** Helper function for the constructors; initialises an OpenGL buffer object, and stores the data in an internal
     * array. */
    template <typename ElementType>
    static unsigned int create_buffer( GLenum buffer_type,
                                       std::vector<ElementType> const & data,
                                       ElementType * internal_data );
};


// Template implementation
template <typename ElementType>
unsigned int Mesh::create_buffer( GLenum const buffer_type,
                                  std::vector<ElementType> const & data,
                                  ElementType * internal_data )
{
    std::ranges::copy( data.begin(), data.end(), internal_data );

    // Create and fill a new OpenGL buffer object
    unsigned int buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( buffer_type, buffer );
    glBufferData( buffer_type, static_cast<long>(data.size()) * sizeof( ElementType ), internal_data, GL_STATIC_DRAW );

    return buffer;
}


#endif //DEMO_TD_MESH_HPP
