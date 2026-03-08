#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"
#include "core/window.hpp"
#include "core/context.hpp"
#include "core/entity_component_system.hpp"

#include "entity/tower.hpp"
#include "entity/tile_highlight.hpp"
#include "system/tile_manager.hpp"

#include <thread>


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    TopContext context {};
    ECS & ecs { *context.get_ecs() };

    LevelContext level { &context };
    std::array<EntityID, TowerData::NumberTypes> towers {};
    for ( unsigned char type { 0 }; type < TowerData::NumberTypes; ++type ) {
        glm::vec3 const position { 0.f, 0.f, -3.f + static_cast<float>(type) };
        towers.at( type ) = Tower::make( static_cast<TowerData::Type>(type), position, &ecs );
    }

    // std::array<EntityID, g_chunk_size> chunk_1 { Tile::make_chunk( { 0, 0, 0 }, &ecs ) };
    // std::array<EntityID, g_chunk_size> chunk_2 { Tile::make_chunk( { 0, 0, 1 }, &ecs ) };
    auto const tiles { ecs.systems.get_system<TileManager>() };
    tiles->add_chunk( { 0, 0, 0 } );
    tiles->add_chunk( { 0, 0, 1 } );

    TileHighlight highlight { &ecs };

    unsigned int frame_count { 0 };
    double last_report { Time::get_time() };

    double constexpr desired_fps { 60. };
    double constexpr desired_loop_length { 1. / desired_fps };
    double loop_length { 0. };

    ecs.systems.run_group( Setup );

    auto const window { context.get_window() };
    while ( not window->is_closing() ) {
        std::this_thread::sleep_for( std::chrono::duration<double> { (desired_loop_length - loop_length) * 0.9925 } );

        double const loop_start { Time::loop_start() };

        glfwPollEvents();
        ecs.systems.run_group( General );
        window->clear();
        ecs.systems.run_group( Render );
        window->render();

        double const loop_end { Time::get_time() };
        loop_length = loop_end - loop_start;

        ++frame_count;
        double const current_time { Time::get_time() };
        if ( double const elapsed_time { current_time - last_report }; elapsed_time >= 1. ) {
            double const fps { static_cast<double>(frame_count) / elapsed_time };
            auto const new_title { std::format( "Demo TD\t[FPS={:.5}]", fps ) };
            glfwSetWindowTitle( glfwGetCurrentContext(), new_title.c_str() );

            frame_count = 0;
            last_report = current_time;
        }
    }

    return 0;
}
