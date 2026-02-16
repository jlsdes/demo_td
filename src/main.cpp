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
#include "entity/tile_highlight.hpp"


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    TopContext context {};
    ECS & ecs { *context.get_ecs() };

    LevelContext level { &context };
    TowerFactory const tower_factory { &ecs };
    std::array<EntityID, TowerData::NumberTypes> towers {};
    for ( unsigned char type { 0 }; type < TowerData::NumberTypes; ++type ) {
        glm::vec3 const position { 0.f, 0.f, -3.f + static_cast<float>(type) };
        towers.at( type ) = tower_factory.build( static_cast<TowerData::Type>(type), position );
    }

    TileFactory const tile_factory { &ecs };
    std::array<EntityID, g_chunk_size> chunk_1 { tile_factory.build_chunk( { 0, 0, 0 } ) };
    std::array<EntityID, g_chunk_size> chunk_2 { tile_factory.build_chunk( { 0, 0, 1 } ) };

    TileHighlight highlight { &ecs };

    unsigned int frame_count { 0 };
    double last_report { Time::get_time() };

    auto const window { context.get_window() };
    while ( not window->is_closing() ) {
        Time::loop_start();
        window->clear();

        glfwPollEvents();
        ecs.systems.run_group( General );
        ecs.systems.run_group( Render );

        window->render();

        ++frame_count;
        double const current_time { Time::get_time() };
        double const elapsed_time { current_time - last_report };
        if ( elapsed_time >= 1. ) {
            Log::debug( "Elapsed time since last report = ", elapsed_time );
            Log::debug( "Frames rendered = ", frame_count, " at ", frame_count / elapsed_time, " FPS" );
            frame_count = 0;
            last_report = current_time;
        }
    }

    return 0;
}
