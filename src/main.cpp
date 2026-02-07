#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"
#include "graphics/window.hpp"
#include "graphics/context.hpp"
#include "graphics/entity_component_system.hpp"

#include "entity/tile_factory.hpp"
#include "entity/tower_factory.hpp"

#include "utils/image.hpp"


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    PBMImageIO pbmer {};
    PGMImageIO pgmer {};
    Image image { pgmer.load_file( main_dir / "binary_grey.pgm" ) };

    auto const pixel { image.pixels.get() };
    if ( not pixel )
        return 0;

    pgmer.set_ascii();
    pgmer.save_file( image, main_dir / "reproduction.pnm" );

    // TopContext context {};
    // ECS & ecs { *context.get_ecs() };
    //
    // LevelContext level { &context };
    // TowerFactory const tower_factory { &ecs };
    // std::array<EntityID, TowerData::NumberTypes> towers {};
    // for ( unsigned char type { 0 }; type < TowerData::NumberTypes; ++type ) {
    //     glm::vec3 const position { 0.f, 0.f, -3.f + static_cast<float>(type) };
    //     towers.at( type ) = tower_factory.build( static_cast<TowerData::Type>(type), position );
    // }
    //
    // TileFactory const tile_factory { &ecs };
    // std::array<EntityID, g_chunk_size> tiles { tile_factory.build_chunk( { 0, 0, 0 } ) };
    //
    // auto const window { context.get_window() };
    // while ( not window->is_closing() ) {
    //     Time::loop_start();
    //     window->clear();
    //
    //     glfwPollEvents();
    //     ecs.systems.run_group( Render );
    //
    //     window->render();
    // }

    return 0;
}
