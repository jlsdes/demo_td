#ifndef DEMO_TD_SHAPES_HPP
#define DEMO_TD_SHAPES_HPP

#include "mesh.hpp"

#include <functional>
#include <vector>


/** Builds meshes, which (at the moment of writing) can't be created or modified easily otherwise. Provides utility
 *  functions for e.g. generating normal vectors, dividing polygons into triangles... Also provides a number of basic
 *  geometric shape generator functions. */
class MeshBuilder {
public:
    /** The constructor, which can be initialised with any amount of initial vertex and face data. */
    explicit MeshBuilder( std::vector<glm::vec3> const & vertices = {},
                          std::vector<std::vector<unsigned int>> const & faces = {},
                          std::vector<glm::vec3> const & normals = {},
                          std::vector<glm::vec3> const & colours = {} );

    /** Default copy/move con-/destructors. */
    MeshBuilder( MeshBuilder const & other ) = default;
    MeshBuilder( MeshBuilder && other ) = default;
    MeshBuilder & operator=( MeshBuilder const & other ) = default;
    MeshBuilder & operator=( MeshBuilder && other ) = default;
    ~MeshBuilder() = default;

    /** Divides the faces of the shape that are not triangles into triangles. */
    MeshBuilder & convert_to_triangles();

    /** Constructs a set of normal vectors, either per face, or per vertex. For the latter case the average of the
     * connected faces is used. This function only looks at the first 3 vertices per face assuming its vertices are all
     * coplanar. */
    MeshBuilder & generate_face_normals();
    MeshBuilder & generate_vertex_normals();

    /** Set every vertex to the given colour. */
    MeshBuilder & colour( glm::vec3 const & colour );

    /** Returns the shape data in a more sesh-friendly format. */
    [[nodiscard]] std::vector<ColourVertex> get_mesh_vertices() const;
    [[nodiscard]] std::vector<unsigned int> get_mesh_face_indices() const;
    [[nodiscard]] Mesh<ColourVertex> get_mesh() const;

    /** Merges another builder's vertices and faces into this builder. */
    MeshBuilder & merge( MeshBuilder const & other );
    MeshBuilder & operator+=( MeshBuilder const & other );
    MeshBuilder operator+( MeshBuilder const & other ) const;

    /** Transforms all vertices currently stored in the builder. */
    void translate( glm::vec3 const & direction );
    void transform( glm::mat3 const & matrix );

    /** Generates a regular polygon with its centre at the origin (0, 0, 0) and one vertex with coordinates (1, 0, 0). All
     *  vertices will lie in the plane where z = 0.
     *
     *  @param nr_corners The number of corners the polygon should have; must be at least 3.
     *  @param normals Whether the polygon should have its normal vectors generated.
     */
    static MeshBuilder regular_polygon( unsigned int nr_corners, bool normals = true );

    /** Generates a rectangle with a given width and height, and with its centre at the origin.
     *
     * @param width The width of the rectangle; must not be zero.
     * @param height The height of the rectangle; must not be zero.
     *  @param normals Whether the polygon should have its normal vectors generated.
     */
    static MeshBuilder rectangle( float width, float height, bool normals = true );
    static MeshBuilder square( float width, bool normals = true );

private:
    static auto constexpr s_zero_elevation = []( float, float ) { return 0.f; };

public:
    /** Generates a large grid of square cells. Each cell will be 'width/columns' wide and 'height/rows' high. */
    static MeshBuilder grid( float size_x,
                             float size_z,
                             unsigned int cells_x,
                             unsigned int cells_z,
                             std::function<float( float, float )> const & elevation = s_zero_elevation,
                             bool normals = true );

    /** Platonic solid generator functions. */
    static MeshBuilder tetrahedron( bool normals = true );
    static MeshBuilder cube( bool normals = true, bool triangles = true );
    static MeshBuilder octahedron( bool normals = true );
    static MeshBuilder dodecahedron( bool normals = true, bool triangles = true );
    static MeshBuilder icosahedron( bool normals = true );

    /** Generates a sphere-like polyhedron. */
    static MeshBuilder sphere( unsigned int n, bool normals = true );

    // private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec3> m_colours;
    std::vector<std::vector<unsigned int>> m_faces;
};


#endif //DEMO_TD_SHAPES_HPP
