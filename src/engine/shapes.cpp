#include "shapes.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <format>
#include <stdexcept>


std::ostream & operator<<( std::ostream & stream, Vector3 const & vector ) {
    return stream << '<' << vector.x << ", " << vector.y << ", " << vector.z << '>';
}

Shape & convert_to_triangles( Shape & shape ) {
    unsigned long const nr_faces_original { shape.faces.size() };
    // Only iterate over the original faces, newly added triangles don't need to be checked for triangleness
    for ( unsigned int i { 0 }; i < nr_faces_original; ++i ) {
        unsigned long const nr_corners { shape.faces[i].size() };
        if ( nr_corners == 3 )
            continue;
        // For now assuming convex polygons
        for ( unsigned int j { 2 }; j < nr_corners - 1; ++j )
            shape.faces.push_back( { shape.faces[i][0], shape.faces[i][j], shape.faces[i][j + 1] } );
        // Keep only the first 3 elements, which is a triangle that was skipped in the for loop
        shape.faces[i].resize( 3 );
        // TODO concave polygons
    }
    return shape;
}

Shape & generate_face_normals( Shape & shape ) {
    shape.normals.clear();
    shape.face_normals = true;
    for ( auto const & face : shape.faces ) {
        // TODO rest of the for loop
    }
    return shape;
}

Shape & generate_vertex_normals( Shape & shape ) {
    shape.normals.clear();
    shape.face_normals = false;
    // TODO rest of the function
    return shape;
}

Shape generate_regular_polygon( unsigned int const nr_corners ) {
    if ( nr_corners < 3 )
        throw std::out_of_range( std::format( "Attempting to create a regular polygon with {} corners.", nr_corners ) );
    Shape shape { {}, { { nr_corners } } };
    shape.vertices.reserve( nr_corners );

    float const angle { glm::two_pi<float>() / static_cast<float>(nr_corners) };
    float current_angle { 0.f };
    for ( unsigned int i { 0 }; i < nr_corners; ++i ) {
        shape.vertices.emplace_back( std::cos( current_angle ), std::sin( current_angle ), 0.f );
        shape.faces.at( 0 ).emplace_back( i );
        current_angle += angle;
    }
    convert_to_triangles( shape );
    return shape;
}

Shape generate_rectangle( float const width, float const height ) {
    if ( height == 0.f )
        throw std::invalid_argument( "Attempting to create a rectangle with zero height." );
    Shape shape {
        {
            { -width / 2, -height / 2, 0.f },
            { -width / 2, height / 2, 0.f },
            { width / 2, height / 2, 0.f },
            { width / 2, -height / 2, 0.f },
        },
        { { 0, 1, 2, 3 } }
    };
    convert_to_triangles( shape );
    return shape;
}

Shape generate_tetrahedron() {}

Shape generate_cube() {
    Shape shape {
        {
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, 0.5f, -0.5f },
            { -0.5f, 0.5f, 0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, 0.5f }
        },
        {
            { 0, 4, 6, 1 },
            { 4, 5, 7, 6 },
            { 5, 1, 3, 7 },
            { 1, 0, 2, 3 },
            { 2, 6, 7, 3 },
            { 1, 5, 4, 0 }
        }
    };
    convert_to_triangles( shape );
    return shape;
}

Shape generate_octahedron() {}

Shape generate_dodecahedron() {}

Shape generate_icosahedron() {}
