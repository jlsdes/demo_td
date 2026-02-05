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
    Image image { 256, 256 };
    unsigned char * pixel_data { image.get( 0, 0 ) };

    for ( unsigned int row { 0 }; row < 256; ++row ) {
        for ( unsigned int col { 0 }; col < 256; ++col ) {
            *pixel_data = static_cast<unsigned char>(row);
            *(pixel_data + 1) = static_cast<unsigned char>(col);
            *(pixel_data + 2) = 0;
            *(pixel_data + 3) = 255;
            pixel_data += 4;
        }
    }

    image.save( get_main_dir() / "test.pbm", Image::BinaryBit );
    image.save( get_main_dir() / "test.pgm", Image::BinaryGray );
    image.save( get_main_dir() / "test.ppm", Image::BinaryPix );

    // auto const main_dir { get_main_dir() };
    // Config::load_config( main_dir / "config.ini" );
    // Log::info( "Loaded config ", (main_dir / "config.ini").string() );
    //
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
