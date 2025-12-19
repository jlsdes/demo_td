#include "shape.hpp"

#include "log.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <format>
#include <stdexcept>


std::ostream & operator<<( std::ostream & stream, Vector3 const & vector ) {
    return stream << '<' << vector.x << ", " << vector.y << ", " << vector.z << '>';
}

std::ostream & operator<<( std::ostream & stream, Vertex const & vertex ) {
    return stream << "<Vertex" << vertex.position << ", " << vertex.normal << ", " << vertex.colour << '>';
}

Shape & Shape::convert_to_triangles() {
    unsigned long const nr_faces_original { faces.size() };
    // Only iterate over the original faces, newly added triangles don't need to be checked for triangleness
    for ( unsigned int i { 0 }; i < nr_faces_original; ++i ) {
        unsigned long const nr_corners { faces.at( i ).size() };
        if ( nr_corners == 3 )
            continue;

        // For now assuming convex polygons
        for ( unsigned int j { 2 }; j < nr_corners - 1; ++j )
            faces.push_back( { faces.at( i ).at( 0 ), faces.at( i ).at( j ), faces.at( i ).at( j + 1 ) } );
        // Keep only the first 3 elements, which is a triangle that was skipped in the for loop
        faces.at( i ).resize( 3 );
        // TODO concave polygons
    }
    return *this;
}

Shape & Shape::generate_face_normals() {
    normals.clear();
    for ( auto const & face : faces ) {
        // TODO rest of the for loop
    }
    return *this;
}

Shape & Shape::generate_vertex_normals() {
    normals.clear();
    // TODO rest of the function
    return *this;
}

std::vector<Vertex> Shape::get_vertices() const {
    bool const has_normals { !normals.empty() };
    if ( !has_normals )
        Log::warning( "Generating vertex data from a shape without any normal vectors." );
    bool const has_colours { !colours.empty() };
    if ( !has_colours )
        Log::warning( "Generating face data from a shape without any colours." );

    std::vector<Vertex> vertex_data { vertices.size() };
    for ( unsigned int i { 0 }; i < vertices.size(); ++i ) {
        Vector3 constexpr zeroes { 0.f };
        auto const normal { has_normals ? normals.at( i ) : zeroes };
        auto const colour { has_colours ? colours.at( i ) : zeroes };
        vertex_data.at( i ) = { vertices.at( i ), normal, colour };
    }
    return vertex_data;
}

std::vector<unsigned int> Shape::get_face_indices() const {
    std::vector<unsigned int> indices( faces.size() * 3 );
    for ( unsigned long i { 0 }; i < faces.size(); ++i ) {
        indices.at( 3 * i + 0 ) = faces.at( i ).at( 0 );
        indices.at( 3 * i + 1 ) = faces.at( i ).at( 1 );
        indices.at( 3 * i + 2 ) = faces.at( i ).at( 2 );
    }
    return indices;
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
    return shape.convert_to_triangles();
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
    return shape.convert_to_triangles();
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
            { 0, 4, 6, 2 },
            { 4, 5, 7, 6 },
            { 5, 1, 3, 7 },
            { 1, 0, 2, 3 },
            { 2, 6, 7, 3 },
            { 1, 5, 4, 0 }
        }
    };
    shape.convert_to_triangles();
    return shape;
}

Shape generate_octahedron() {}

Shape generate_dodecahedron() {}

Shape generate_icosahedron() {}
