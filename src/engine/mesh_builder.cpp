#include "mesh_builder.hpp"

#include "log.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cmath>
#include <format>
#include <numbers>
#include <ranges>
#include <stdexcept>


MeshBuilder::MeshBuilder( std::vector<glm::vec3> const & vertices,
                          std::vector<std::vector<unsigned int>> const & faces,
                          std::vector<glm::vec3> const & normals,
                          std::vector<glm::vec3> const & colours )
    : m_vertices { vertices }, m_normals { normals }, m_colours { colours }, m_faces { faces } {}

/** Returns whether a face is a convex polygon. */
bool is_convex( std::vector<glm::vec3> const & vertices, std::vector<unsigned int> const & face ) {
    glm::vec3 current_vertex { vertices.at( face.at( 0 ) ) };
    glm::vec3 current_edge { current_vertex - vertices.at( face.at( face.size() - 1 ) ) };
    glm::vec3 const last_edge { vertices.at( face.at( face.size() - 1 ) ) - vertices.at( face.at( face.size() - 2 ) ) };
    glm::vec3 const last_normal { glm::normalize( glm::cross( last_edge, current_edge ) ) };

    for ( unsigned int i { 1 }; i < face.size(); ++i ) {
        glm::vec3 const next_vertex { vertices.at( face.at( i ) ) };
        glm::vec3 const next_edge { next_vertex - current_vertex };

        if ( glm::length( glm::normalize( glm::cross( current_edge, next_edge ) ) - last_normal ) > 0.00001 )
            return false;

        current_vertex = next_vertex;
        current_edge = next_edge;
    }
    return true;
}

MeshBuilder & MeshBuilder::convert_to_triangles() {
    unsigned long const nr_faces_original { m_faces.size() };
    // Only iterate over the original faces, newly added triangles don't need to be checked for triangleness
    for ( unsigned int i { 0 }; i < nr_faces_original; ++i ) {
        unsigned long const nr_corners { m_faces.at( i ).size() };
        if ( nr_corners == 3 )
            continue;

        if ( !is_convex( m_vertices, m_faces.at( i ) ) )
            Log::warning( "Concave polygon detected; this is not fully supported." );

        for ( unsigned int j { 2 }; j < nr_corners - 1; ++j )
            m_faces.push_back( { m_faces.at( i ).at( 0 ), m_faces.at( i ).at( j ), m_faces.at( i ).at( j + 1 ) } );
        // Keep only the first 3 elements, which is a triangle that was skipped in the for loop
        m_faces.at( i ).resize( 3 );
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
    // Assuming the vertices are given in counter-clockwise order
    return glm::normalize( glm::cross( vertex_1 - vertex_0, vertex_2 - vertex_0 ) );
}

MeshBuilder & MeshBuilder::generate_face_normals() {
    // When duplicating vertices, also copy colours if they are currently matching the vertices 1:1
    bool const copy_colours { m_vertices.size() == m_colours.size() };
    // Vertices might have different normals for each face they're a corner of; duplicating vertices can solve this
    auto const faces_per_vertex { get_faces_per_vertex( m_faces, m_vertices.size() ) };
    for ( unsigned int vertex_index { 0 }; vertex_index < faces_per_vertex.size(); ++vertex_index ) {
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
    std::vector<glm::vec3> face_normals { m_faces.size() };
    for ( auto const & [face_index, face] : std::ranges::enumerate_view( m_faces ) )
        face_normals.at( face_index ) = compute_normal( m_vertices, face );
    auto const faces_per_vertex { get_faces_per_vertex( m_faces, m_vertices.size() ) };

    m_normals.resize( m_vertices.size() );
    for ( auto const & [vertex_index, vertex] : std::ranges::enumerate_view( m_vertices ) ) {
        glm::vec3 normal_sum { 0.f, 0.f, 0.f };
        for ( auto const [face_index, _] : faces_per_vertex.at( vertex_index ) )
            normal_sum += face_normals.at( face_index );
        m_normals.at( vertex_index ) = normal_sum / static_cast<float>(faces_per_vertex.at( vertex_index ).size());
    }
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

MeshBuilder & MeshBuilder::merge( MeshBuilder const & other ) {
    if ( m_normals.empty() != other.m_normals.empty() )
        Log::warning( "Merging two MeshBuilders with mismatched normal presence; dropping existing normals." );
    if ( m_colours.empty() != other.m_colours.empty() )
        Log::warning( "Merging two MeshBuilders with mismatched colour presence; dropping existing colours." );

    auto const offset { m_vertices.size() };
    m_vertices.insert( m_vertices.end(), other.m_vertices.cbegin(), other.m_vertices.cend() );
    if ( !m_normals.empty() && !other.m_normals.empty() )
        m_normals.insert( m_normals.end(), other.m_normals.cbegin(), other.m_normals.cend() );
    if ( !m_colours.empty() && !other.m_colours.empty() )
        m_colours.insert( m_colours.end(), other.m_colours.cbegin(), other.m_colours.cend() );

    for ( auto face : other.m_faces ) {
        for ( unsigned int & index : face )
            index += offset;
        m_faces.push_back( face );
    }
    return *this;
}

MeshBuilder & MeshBuilder::operator+=( MeshBuilder const & other ) {
    return merge( other );
}

MeshBuilder MeshBuilder::operator+( MeshBuilder const & other ) const {
    MeshBuilder copy { *this };
    return copy += other;
}

void MeshBuilder::translate( glm::vec3 const & direction ) {
    for ( glm::vec3 & vertex : m_vertices )
        vertex += direction;
}

void MeshBuilder::transform( glm::mat3 const & matrix ) {
    for ( glm::vec3 & vertex : m_vertices )
        vertex = matrix * vertex;

    if ( !m_normals.empty() ) {
        glm::mat3 const normal_transform { glm::transpose( glm::inverse( matrix ) ) };
        for ( glm::vec3 & normal : m_normals )
            normal = normal_transform * normal;
    }
}

MeshBuilder MeshBuilder::regular_polygon( unsigned int const nr_corners ) {
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

MeshBuilder MeshBuilder::rectangle( float const width, float const height ) {
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

MeshBuilder MeshBuilder::tetrahedron( bool const normals ) {
    MeshBuilder tetrahedron {
        {
            { -1.f, -1.f, 1.f },
            { -1.f, 1.f, -1.f },
            { 1.f, -1.f, -1.f },
            { 1.f, 1.f, 1.f },
        },
        {
            { 0, 2, 3 },
            { 0, 3, 1 },
            { 3, 2, 1 },
            { 2, 0, 1 },
        }
    };
    // Rescale the cube to be inscribed within a sphere of radius 1
    for ( auto & vertex : tetrahedron.m_vertices )
        vertex /= std::numbers::sqrt3_v<float>;

    if ( normals )
        tetrahedron.generate_face_normals();
    return tetrahedron;
}

MeshBuilder MeshBuilder::cube( bool const normals, bool const triangles ) {
    MeshBuilder cube {
        {
            { -1.f, -1.f, 1.f },
            { -1.f, -1.f, -1.f },
            { -1.f, 1.f, 1.f },
            { -1.f, 1.f, -1.f },
            { 1.f, -1.f, 1.f },
            { 1.f, -1.f, -1.f },
            { 1.f, 1.f, 1.f },
            { 1.f, 1.f, -1.f },
        },
        {
            { 0, 4, 6, 2 },
            { 4, 5, 7, 6 },
            { 5, 1, 3, 7 },
            { 1, 0, 2, 3 },
            { 2, 6, 7, 3 },
            { 1, 5, 4, 0 },
        }
    };
    // Rescale the cube to be inscribed within a sphere of radius 1
    for ( auto & vertex : cube.m_vertices )
        vertex /= std::numbers::sqrt3_v<float>;

    if ( normals )
        cube.generate_face_normals();
    if ( triangles )
        cube.convert_to_triangles();
    return cube;
}

MeshBuilder MeshBuilder::octahedron( bool normals ) {
    MeshBuilder octahedron {
        {
            { 0.f, -1.f, 0.f }, // Bottom vertex
            { -1.f, 0.f, 0.f }, // Middle vertices in clockwise order when looking from the top
            { 0.f, 0.f, -1.f },
            { 1.f, 0.f, 0.f },
            { 0.f, 0.f, 1.f },
            { 0.f, 1.f, 0.f }, // Top vertex
        },
        {
            { 0, 1, 2 },
            { 0, 2, 3 },
            { 0, 3, 4 },
            { 0, 4, 1 },
            { 5, 2, 1 },
            { 5, 3, 2 },
            { 5, 4, 3 },
            { 5, 1, 4 },
        }
    };
    if ( normals )
        octahedron.generate_face_normals();
    return octahedron;
}

float constexpr phi { std::numbers::phi_v<float> };
float constexpr phi_inv { 1.f / phi };

MeshBuilder MeshBuilder::dodecahedron( bool const normals, bool const triangles ) {
    MeshBuilder dodecahedron {
        // Coordinates taken from wikipedia (https://en.wikipedia.org/wiki/Regular_dodecahedron)
        {
            { -1.f, -1.f, -1.f }, // Cube vertices [0-7]
            { -1.f, -1.f, 1.f },
            { -1.f, 1.f, -1.f },
            { -1.f, 1.f, 1.f },
            { 1.f, -1.f, -1.f },
            { 1.f, -1.f, 1.f },
            { 1.f, 1.f, -1.f },
            { 1.f, 1.f, 1.f },
            { 0.f, -phi, -phi_inv }, // yz-plane rectangle vertices [8-11]
            { 0.f, -phi, phi_inv },
            { 0.f, phi, -phi_inv },
            { 0.f, phi, phi_inv },
            { -phi_inv, 0.f, -phi }, // xz-plane rectangle vertices [12-15]
            { -phi_inv, 0.f, phi },
            { phi_inv, 0.f, -phi },
            { phi_inv, 0.f, phi },
            { -phi, -phi_inv, 0.f }, // xy-plane rectangle vertices [16-19]
            { -phi, phi_inv, 0.f },
            { phi, -phi_inv, 0.f },
            { phi, phi_inv, 0.f },
        },
        // Each face can be defined by the following set of vertices (in a slightly different order):
        // - one of the rectangle vertices [8-19].
        // - the two cube vertices whose signs match the non-zero coordinate values of the first vertex.
        // - the two rectangle vertices which can be found by transforming the original (i.e. first vertex) coordinates
        //   - the original phi coordinate becomes zero.
        //   - the original phi_inv coordinate becomes phi.
        //   - the original zero coordinate becomes +/- phi_inv.
        {
            // For example, with first vertex 8 = ( 0, -phi, -phi_inv )
            // The two cube vertices 0 = ( -1, -1, -1 ) and 4 = ( 1, -1, -1 )
            // The two other rectangle vertices 12 = ( -phi_inv, 0, -phi ) and 14 = ( phi_inv, 0, -phi )
            { 8, 0, 12, 14, 4 },
            { 9, 5, 15, 13, 1 },
            { 10, 6, 14, 12, 2 },
            { 11, 3, 13, 15, 7 },
            { 12, 0, 16, 17, 2 },
            { 13, 3, 17, 16, 1 },
            { 14, 6, 19, 18, 4 },
            { 15, 5, 18, 19, 7 },
            { 16, 0, 8, 9, 1 },
            { 17, 3, 11, 10, 2 },
            { 18, 5, 9, 8, 4 },
            { 19, 6, 10, 11, 7 },
        }
    };
    // Rescale the dodecahedron to be inscribed within a sphere of radius 1
    for ( auto & vertex : dodecahedron.m_vertices )
        vertex /= std::numbers::sqrt3_v<float>;

    if ( normals )
        dodecahedron.generate_face_normals();
    if ( triangles )
        dodecahedron.convert_to_triangles();
    return dodecahedron;
}

MeshBuilder MeshBuilder::icosahedron( bool const normals ) {
    MeshBuilder icosahedron {
        {
            { 0.f, -1.f, -phi }, // yz-plane rectangle vertices [0-3]
            { 0.f, -1.f, phi },
            { 0.f, 1.f, -phi },
            { 0.f, 1.f, phi },
            { -phi, 0.f, -1.f }, // xz-plane rectangle vertices [4-7]
            { -phi, 0.f, 1.f },
            { phi, 0.f, -1.f },
            { phi, 0.f, 1.f },
            { -1.f, -phi, 0.f }, // xy-plane rectangle vertices [8-11]
            { -1.f, phi, 0.f },
            { 1.f, -phi, 0.f },
            { 1.f, phi, 0.f },
        },
        // There are two types of face 'definitions' used here.
        // For every vertex, exactly one face exists that links it with the two other vertices which have values +/-phi
        //  where the first vertex has value +/-1 (the signs for these specific coordinate values must match).
        // Every group of three vertices, where all signs match on every coordinate value with 0 as a universal match,
        //  also defines a single face.
        {
            // E.g. vertex 0 = ( 0, -1, -phi ) and its neighbours 10 = ( 1, -phi, 0 ) and 8 = ( -1, -phi, 0 )
            { 0, 10, 8 },
            { 1, 8, 10 },
            { 2, 9, 11 },
            { 3, 11, 9 },
            { 4, 2, 0 },
            { 5, 1, 3 },
            { 6, 0, 2 },
            { 7, 3, 1 },
            { 8, 5, 4 },
            { 9, 4, 5 },
            { 10, 6, 7 },
            { 11, 7, 6 },
            // E.g. all vertices with coordinate values x <= 0, y <= 0, z <= 0
            { 0, 8, 4 }, // ---
            { 1, 5, 8 }, // --+
            { 2, 4, 9 }, // -+-
            { 3, 9, 5 }, // -++
            { 0, 6, 10 }, // +--
            { 1, 10, 7 }, // +-+
            { 2, 11, 6 }, // ++-
            { 3, 7, 11 }, // +++
        }
    };
    // Rescale the dodecahedron to be inscribed within a sphere of radius 1
    for ( auto & vertex : icosahedron.m_vertices )
        vertex /= std::sqrt( phi * phi + 1 );

    if ( normals )
        icosahedron.generate_face_normals();
    return icosahedron;
}

/// Stores an edge by its two vertex endpoints.
using Edge = std::pair<unsigned int, unsigned int>;
using EdgeData = std::pair<unsigned int, unsigned int>;
/// Stores an iterable range containing the indices of newly created vertices along edges.
/// This array wastes some memory space, as not all pairs of vertices define an actually existing edge.
using EdgeMap = std::array<EdgeData, 144>;

/// Utility struct holding relevant information related to dividing a face into multiple triangles.
struct FaceData {
    MeshBuilder & sphere;
    EdgeMap & edges;
    std::vector<unsigned int> vertices;
    unsigned int n;
};

/** Returns a reference to the vertex's index by (part of) its barycentric coordinates. This function assumes that the
 *  barycentric coordinate values sum to n, and thus does not need the first coordinate u. */
unsigned int & vertex_reference( FaceData & data, unsigned int const v, unsigned int const w ) {
    // Vertices are stored in this order: (n, 0, 0), (n-1, 1, 0), (n-1, 0, 1), (n-2, 2, 0) ... (0, 0, n)
    // Row 0:           (n, 0, 0)
    // Row 1:       (n-1, 1, 0) (n-1, 0, 1)
    // Row 2:   (n-2, 2, 0) ...
    // ...
    // Row n: (0, n, 0) ... (0, 0, n)
    unsigned int const row { v + w };
    unsigned int const row_index { row * (row + 1) / 2 };
    return data.vertices.at( row_index + w );
}

unsigned int const & vertex_reference( FaceData const & data, unsigned int const v, unsigned int const w ) {
    return data.vertices.at( (v + w) * (v + w + 1) / 2 + w );
}

/** Returns a reference to the edge data associated with the given edge in the edge map/array. */
EdgeData & edge_reference( FaceData & data, Edge const & edge, bool const reverse = false ) {
    return data.edges.at( reverse ? edge.first * 12 + edge.second : edge.second * 12 + edge.first );
}

EdgeData const & edge_reference( FaceData const & data, Edge const & edge, bool const reverse = false ) {
    return data.edges.at( reverse ? edge.first * 12 + edge.second : edge.second * 12 + edge.first );
}

/** Returns the index data on the new vertices along an edge, and creates those vertices if necessary. */
EdgeData get_edge_vertices( FaceData & data, Edge const & edge ) {
    auto & edge_data { edge_reference( data, edge ) };
    if ( edge_data.first )
        return edge_data;
    edge_data = { data.sphere.m_vertices.size(), true }; // Modifies data.edges as well btw

    glm::vec3 position { data.sphere.m_vertices.at( edge.first ) };
    glm::vec3 const step { (data.sphere.m_vertices.at( edge.second ) - position) / static_cast<float>(data.n) };
    for ( unsigned int i { 1 }; i < data.n; ++i )
        data.sphere.m_vertices.emplace_back( glm::normalize( position += step ) );

    // The edge should be registered in both directions
    edge_reference( data, edge, true ) = { data.sphere.m_vertices.size() - 1, false };
    return edge_data;
}

/** Creates new vertices along each of the edges of the face, or retrieves them if they already exist. This function
 *  also adds all vertices on an edge/corner to the 'vertices' vector in 'data'. */
void create_edge_vertices( FaceData & data, std::vector<unsigned int> const & face ) {
    for ( unsigned int i { 0 }; i < 3; ++i ) {
        unsigned int bary_coordinate[3] {};
        bary_coordinate[i] = data.n; // Starting at a corner
        vertex_reference( data, bary_coordinate[1], bary_coordinate[2] ) = face.at( i );

        // Retrieve/create all vertices along this edge of the face
        auto [index, ascending] { get_edge_vertices( data, { face.at( i ), face.at( (i + 1) % 3 ) } ) };
        for ( unsigned int j { 1 }; j < data.n; ++j ) {
            --bary_coordinate[i];
            ++bary_coordinate[(i + 1) % 3];
            vertex_reference( data, bary_coordinate[1], bary_coordinate[2] ) = index;
            index += ascending ? 1 : -1;
        }
    }
}

/** Creates all internal vertices (i.e. vertices that don't lie on an edge/corner) in a triangle. */
void create_internal_vertices( FaceData & data, std::vector<unsigned int> const & face ) {
    glm::vec3 const vertex_0 { data.sphere.m_vertices.at( face.at( 0 ) ) };
    glm::vec3 const vertex_1 { data.sphere.m_vertices.at( face.at( 1 ) ) };
    glm::vec3 const vertex_2 { data.sphere.m_vertices.at( face.at( 2 ) ) };
    glm::vec3 const step_v { (vertex_1 - vertex_0) / static_cast<float>(data.n) };
    glm::vec3 const step_w { (vertex_2 - vertex_0) / static_cast<float>(data.n) };
    for ( unsigned int v { 1 }; v < data.n; ++v ) {
        glm::vec3 position { vertex_0 + static_cast<float>(v) * step_v };
        for ( unsigned int w { 1 }; w < data.n - v; ++w ) {
            vertex_reference( data, v, w ) = data.sphere.m_vertices.size();
            data.sphere.m_vertices.emplace_back( glm::normalize( position += step_w ) );
        }
    }
}

/** Creates new triangles between all newly created vertices. */
void create_triangles( FaceData const & data, std::vector<std::vector<unsigned int>> & faces ) {
    for ( unsigned int v { 0 }; v < data.n; ++v ) {
        for ( unsigned int w { 0 }; w < data.n - v; ++w ) {
            //            (u, v, w)
            //                /\
            // (u-1, v+1, w) /__\ (u-1, v, w+1)
            //               \  /
            //                \/
            //         (u-2, v+1, w+1)
            faces.push_back( {
                vertex_reference( data, v, w ), vertex_reference( data, v + 1, w ), vertex_reference( data, v, w + 1 )
            } );
            if ( w != data.n - v - 1 )
                faces.push_back( {
                    vertex_reference( data, v, w + 1 ), vertex_reference( data, v + 1, w ),
                    vertex_reference( data, v + 1, w + 1 )
                } );
        }
    }
}

MeshBuilder MeshBuilder::sphere( unsigned int const n, bool const normals ) {
    // Starting from a basic icosahedron with its original 12 vertices and 20 faces
    auto sphere = icosahedron( false );
    std::vector<std::vector<unsigned int>> faces {};

    EdgeMap edges; // Initialised with 0 and false values; 0 cannot appear otherwise, so it indicates empty cells

    for ( auto const & face : sphere.m_faces ) {
        std::vector<unsigned int> vertices( (n + 1) * (n + 2) / 2 );
        FaceData face_data { sphere, edges, std::move( vertices ), n };

        create_edge_vertices( face_data, face );
        create_internal_vertices( face_data, face );
        create_triangles( face_data, faces );
    }
    sphere.m_faces = faces;

    if ( normals )
        sphere.generate_vertex_normals();
    return sphere;
}
