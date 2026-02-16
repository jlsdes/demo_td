#ifndef DEMO_TD_TERRAIN_TILE_HPP
#define DEMO_TD_TERRAIN_TILE_HPP

#include "component.hpp"

#include <bitset>
#include <numbers>

#include <glm/glm.hpp>


static_assert( sizeof( unsigned int ) == 4 ); // Shouldn't fail on common (64 bit) systems

/// The coordinate of the tile in the skewed space, where the triangular tiles are mapped to a square-like grid (i.e.
/// every square in this new grid contains exactly two skewed triangles). This is also used in simplex noise.
struct SkewedCoordinate {
    int x : 31;
    int y : 31;
    unsigned int half : 1; // Which triangular half of the square this tile maps to
};


struct TerrainTile : Component {

    enum Flag : unsigned char {
        HasTower,
        IsPassable,
        NumberFlags, // Indicates the number of flags, but is not considered a valid enum value
    };

    std::bitset<Flag::NumberFlags> flags { 0b00 };
    SkewedCoordinate tile_id { 0, 0, 0 };
};


/** Computes the ID of the tile the given (x, z) coordinate is in, with y=0. */
SkewedCoordinate constexpr position_to_tile( float const x, float const z ) {
    float constexpr F { 0.5f * (std::numbers::sqrt3_v<float> - 1) };
    float const skewing_factor { F * (x + z) };
    float const x_skewed { x + skewing_factor };
    float const z_skewed { z + skewing_factor };
    float const x_floor { std::floor(x_skewed) };
    float const z_floor { std::floor(z_skewed) };
    bool const half { x_skewed - x_floor < z_skewed - z_floor };
    return { static_cast<int>(x_floor), static_cast<int>(z_floor), half };
}

SkewedCoordinate constexpr position_to_tile( glm::vec3 const & position ) {
    return position_to_tile( position.x, position.z );
}

/** Returns the world coordinate of the 'bottom left' corner of the tile. */
glm::vec3 constexpr tile_position( SkewedCoordinate const & tile_id ) {
    float constexpr G { 0.5f - std::numbers::sqrt3_v<float> / 6.f };
    float const x { static_cast<float>(tile_id.x) };
    float const y { static_cast<float>(tile_id.y) };
    float const unskewing_factor { G * (x + y) };
    return { x - unskewing_factor, 0.f, y - unskewing_factor };
}

glm::vec3 constexpr tile_position( int const x, int const z ) {
    return tile_position( { x, z, 0 } );
}


#endif //DEMO_TD_TERRAIN_TILE_HPP
