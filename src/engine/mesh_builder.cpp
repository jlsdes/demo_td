#include "mesh_builder.hpp"

#include "log.hpp"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <format>
#include <stdexcept>


MeshBuilder::MeshBuilder( std::vector<Vector3> const & vertices,
                          std::vector<std::vector<unsigned int>> const & faces,
                          std::vector<Vector3> const & normals,
                          std::vector<Vector3> const & colours )
    : m_vertices { vertices }, m_normals { normals }, m_colours { colours }, m_faces { faces } {}

MeshBuilder & MeshBuilder::convert_to_triangles() {
    unsigned long const nr_faces_original { m_faces.size() };
    // Only iterate over the original faces, newly added triangles don't need to be checked for triangleness
    for ( unsigned int i { 0 }; i < nr_faces_original; ++i ) {
        unsigned long const nr_corners { m_faces.at( i ).size() };
        if ( nr_corners == 3 )
            continue;

        // For now assuming convex polygons
        for ( unsigned int j { 2 }; j < nr_corners - 1; ++j )
            m_faces.push_back( { m_faces.at( i ).at( 0 ), m_faces.at( i ).at( j ), m_faces.at( i ).at( j + 1 ) } );
        // Keep only the first 3 elements, which is a triangle that was skipped in the for loop
        m_faces.at( i ).resize( 3 );
        // TODO concave polygons
    }
    return *this;
}

MeshBuilder & MeshBuilder::generate_face_normals() {
    m_normals.clear();
    for ( auto const & face : m_faces ) {
        // TODO rest of the for loop
    }
    return *this;
}

MeshBuilder & MeshBuilder::generate_vertex_normals() {
    m_normals.clear();
    // TODO rest of the function
    return *this;
}

std::vector<Vertex> MeshBuilder::get_mesh_vertices() const {
    bool const has_normals { !m_normals.empty() };
    if ( !has_normals )
        Log::warning( "Generating vertex data from a shape without any normal vectors." );
    bool const has_colours { !m_colours.empty() };
    if ( !has_colours )
        Log::warning( "Generating face data from a shape without any colours." );

    std::vector<Vertex> vertex_data { m_vertices.size() };
    for ( unsigned int i { 0 }; i < m_vertices.size(); ++i ) {
        Vector3 constexpr zeroes { 0.f };
        auto const normal { has_normals ? m_normals.at( i ) : zeroes };
        auto const colour { has_colours ? m_colours.at( i ) : zeroes };
        vertex_data.at( i ) = { m_vertices.at( i ), normal, colour };
    }
    return vertex_data;
}

std::vector<unsigned int> MeshBuilder::get_mesh_face_indices() const {
    std::vector<unsigned int> indices( m_faces.size() * 3 );
    for ( unsigned long i { 0 }; i < m_faces.size(); ++i ) {
        indices.at( 3 * i + 0 ) = m_faces.at( i ).at( 0 );
        indices.at( 3 * i + 1 ) = m_faces.at( i ).at( 1 );
        indices.at( 3 * i + 2 ) = m_faces.at( i ).at( 2 );
    }
    return indices;
}

Mesh MeshBuilder::get_mesh() const {
    return Mesh { get_mesh_vertices(), get_mesh_face_indices(), GL_TRIANGLES };
}

MeshBuilder MeshBuilder::generate_regular_polygon( unsigned int const nr_corners ) {
    if ( nr_corners < 3 )
        throw std::out_of_range( std::format( "Attempting to create a regular polygon with {} corners.", nr_corners ) );
    MeshBuilder shape { {}, { { nr_corners } } };
    shape.m_vertices.reserve( nr_corners );

    float const angle { glm::two_pi<float>() / static_cast<float>(nr_corners) };
    float current_angle { 0.f };
    for ( unsigned int i { 0 }; i < nr_corners; ++i ) {
        shape.m_vertices.emplace_back( std::cos( current_angle ), std::sin( current_angle ), 0.f );
        shape.m_faces.at( 0 ).emplace_back( i );
        current_angle += angle;
    }
    return shape.convert_to_triangles();
}

MeshBuilder MeshBuilder::generate_rectangle( float const width, float const height ) {
    if ( height == 0.f )
        throw std::invalid_argument( "Attempting to create a rectangle with zero height." );
    MeshBuilder shape {
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

MeshBuilder MeshBuilder::generate_tetrahedron() {}

MeshBuilder MeshBuilder::generate_cube() {
    MeshBuilder shape {
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

MeshBuilder MeshBuilder::generate_octahedron() {}

MeshBuilder MeshBuilder::generate_dodecahedron() {}

MeshBuilder MeshBuilder::generate_icosahedron() {}
