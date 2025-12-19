#ifndef DEMO_TD_SHAPES_HPP
#define DEMO_TD_SHAPES_HPP

#include <vector>
#include <ostream>


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


struct Shape {
    std::vector<Vector3> vertices;
    std::vector<std::vector<unsigned int> > faces;
    std::vector<Vector3> normals = {};
    std::vector<Vector3> colours = {};

    /** Divides the faces of the shape that are not triangles into triangles. */
    Shape & convert_to_triangles();

    /** Constructs a set of normal vectors, either per face, or per vertex. For the latter case the average of the
     * connected faces is used. This function only looks at the first 3 vertices per face, assuming all its vertices are
     * coplanar. */
    Shape & generate_face_normals();
    Shape & generate_vertex_normals();

    /** Returns the shape data in a more sesh-friendly format. */
    [[nodiscard]] std::vector<Vertex> get_vertices() const;
    [[nodiscard]] std::vector<unsigned int> get_face_indices() const;
};


/** Generates a regular polygon with its centre at the origin (0, 0, 0) and one vertex with coordinates (1, 0, 0). All
 *  vertices will lie in the plane where z = 0.
 *
 *  @param nr_corners The number of corners the polygon should have; must be at least 3.
 */
Shape generate_regular_polygon( unsigned int nr_corners );

/** Generates a rectangle with a given width and height, and with its centre at the origin.
 *
 * @param width The width of the rectangle; must not be zero.
 * @param height The height of the rectangle; must not be zero.
 */
Shape generate_rectangle( float width, float height );

/** Generates the Platonic solids. */
Shape generate_tetrahedron();
Shape generate_cube();
Shape generate_octahedron();
Shape generate_dodecahedron();
Shape generate_icosahedron();


#endif //DEMO_TD_SHAPES_HPP