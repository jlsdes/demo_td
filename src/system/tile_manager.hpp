#ifndef DEMO_TD_TILE_MANAGER_HPP
#define DEMO_TD_TILE_MANAGER_HPP

#include "system.hpp"

#include "component/terrain_tile.hpp"


/// The length of the chunk as in the number of individual tile borders that make up one chunk border.
unsigned int constexpr g_chunk_length { 16 };
/// The number of tiles in a single chunk, which is just length^2.
unsigned int constexpr g_chunk_size { g_chunk_length * g_chunk_length };


class TileManager : public System {
public:
    explicit TileManager( ECS * ecs );
    ~TileManager() override = default;

    void run() override;

    /** Creates a single tile of the triangular grid.
     *
     * The tile ID is the coordinate of the "bottom left" corner of the tile after a skewing operation. This skewing
     * operation transforms the triangular grid into a square-like grid, with each square consisting of two skewed
     * triangles. The skew moves points along the vector (1, 1) with a magnitude relative to the distance of the point
     * to the diagonal y=-x (in 2D space).
     *
     * Also note that the SkewedCoordinate is a 2D coordinate (x, y), which is mapped to the horizontal in 3D space like
     * this (x, 0, y).
     *
     * +--+  After skewing the triangles, we get squares that look like this.
     * | /|  The tile ID's 'half' attribute indicates which of the two triangles it's identifying.
     * |/ |  0 indicates the top left triangle.
     * +--+  1 indicates the bottom right triangle.
     * ^ The tile ID's (x, y) coordinate points at this corner.
     */
    void add_tile( SkewedCoordinate tile_id );

    /** Builds a triangular chunk of triangles, with the chunks laid out in the same pattern as the individual tiles. */
    void add_chunk( SkewedCoordinate chunk_id );

    /** Mechanism for signalling to the renderer that the tiles have been updated. Only the renderer should be calling
     *  clear_updates(). */
    [[nodiscard]] bool has_updated() const;
    void clear_updates();

private:
    /// Set after any changes to the tiles, and cleared once the renderer has updated its view.
    bool m_updated;
};


#endif //DEMO_TD_TILE_MANAGER_HPP