#include "mesh_builder.hpp"

#include "log.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <format>
#include <ranges>
#include <stdexcept>


MeshBuilder::MeshBuilder( std::vector<glm::vec3> const & vertices,
                          std::vector<std::vector<unsigned int>> const & faces,
                          std::vector<glm::vec3> const & normals,
                          std::vector<glm::vec3> const & colours )
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

struct ValueLocation {
    unsigned int face_index; // The index of the face in 'm_faces'.
    unsigned int face_vertex_index; // The index of the vertex in 'm_faces.at( face_index )'.
};

/// Returns the indices of the faces each vertex is part of, and the indices of the vertex index within the faces' lists
/// of vertices.
std::vector<std::vector<ValueLocation>> get_faces_per_vertex( std::vector<std::vector<unsigned int>> const & faces,
                                                              unsigned long const nr_vertices ) {
    std::vector<std::vector<ValueLocation>> faces_per_vertex { nr_vertices };
    for ( unsigned int face_index { 0 }; face_index < faces.size(); ++face_index )
        for ( auto const [index, vertex_index] : std::ranges::enumerate_view( faces.at( face_index ) ) )
            faces_per_vertex.at( vertex_index ).emplace_back( face_index, index );
    return faces_per_vertex;
}

/// Computes the (normalised) normal vector for a given face. This function only looks at the first 3 vertices given in
/// its index vector.
glm::vec3 compute_normal( std::vector<glm::vec3> const & vertices, std::vector<unsigned int> const & face ) {
    auto const vertex_0 { vertices.at( face.at( 0 ) ) };
    auto const vertex_1 { vertices.at( face.at( 1 ) ) };
    auto const vertex_2 { vertices.at( face.at( 2 ) ) };
    return glm::normalize( glm::cross( vertex_2 - vertex_0, vertex_1 - vertex_0 ) );
}

MeshBuilder & MeshBuilder::generate_face_normals() {
    // When duplicating vertices, also copy colours if they are currently matching the vertices 1:1
    bool const copy_colours { m_vertices.size() == m_colours.size() };
    // Vertices might have different normals for each face they're a corner of; duplicating vertices can solve this
    auto const faces_per_vertex { get_faces_per_vertex( m_faces, m_vertices.size() ) };
    for ( unsigned int vertex_index {0}; vertex_index < faces_per_vertex.size(); ++vertex_index ) {
        auto const & face_indices { faces_per_vertex.at( vertex_index ) };
        if ( face_indices.empty() )
            Log::debug( "Vertex found without any faces attached." );

        for ( unsigned int i { 1 }; i < face_indices.size(); ++i ) {
            auto const [face_index, face_vertex_index] { face_indices.at( i ) };
            m_faces.at( face_index ).at( face_vertex_index ) = m_vertices.size();
            m_vertices.emplace_back( m_vertices.at( vertex_index ) );
            if ( copy_colours )
                m_colours.emplace_back( m_colours.at( vertex_index ) );
        }
    }
    m_normals.resize( m_vertices.size() );

    for ( auto const & face : m_faces ) {
        glm::vec3 const normal { compute_normal( m_vertices, face ) };
        for ( auto const vertex_index : face )
            m_normals.at( vertex_index ) = normal;
    }
    return *this;
}

MeshBuilder & MeshBuilder::generate_vertex_normals() {
    m_normals.clear();
    // TODO rest of the function
    return *this;
}

/// Helper function for get_mesh_vertices.
bool is_valid_attribute_vector( std::string const & name,
                                std::vector<glm::vec3> const & data,
                                std::vector<glm::vec3> const & vertices
) {
    auto const size { data.size() };
    auto const expected { vertices.size() };
    if ( size == 0 )
        Log::warning( "Generating vertex data, but the normal vectors are missing." );
    else if ( size != expected )
        Log::error( "Generating vertex data, but the number of vertices (", expected,
                    ") does not match the number of ", name, " vectors (", size, ")." );
    return size == expected;
}

std::vector<Vertex> MeshBuilder::get_mesh_vertices() const {
    bool const has_normals { is_valid_attribute_vector( "normal", m_normals, m_vertices ) };
    bool const has_colours { is_valid_attribute_vector( "colour", m_colours, m_vertices ) };

    std::vector<Vertex> vertex_data { m_vertices.size() };
    for ( unsigned int i { 0 }; i < m_vertices.size(); ++i ) {
        glm::vec3 constexpr zeroes { 0.f };
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
