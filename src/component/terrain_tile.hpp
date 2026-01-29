#ifndef DEMO_TD_TERRAIN_TILE_HPP
#define DEMO_TD_TERRAIN_TILE_HPP

#include "component.hpp"

#include <bitset>


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


#endif //DEMO_TD_TERRAIN_TILE_HPP