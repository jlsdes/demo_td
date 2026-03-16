#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/window.hpp"
#include "core/context.hpp"

#include "entity/tower.hpp"

#include "system/system_manager.hpp"

#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"

#include <thread>


int main() {
    auto const main_dir { get_main_dir() };
    Config::load_config( main_dir / "config.ini" );
    Log::info( "Loaded config ", (main_dir / "config.ini").string() );

    TopContext context {};

    LevelContext level { &context };
    std::array<EntityID, TowerType::NumberTypes> towers {};
    for ( unsigned char type { 0 }; type < TowerType::NumberTypes; ++type ) {
        glm::vec3 const position { 0.f, 0.f, -3.f + static_cast<float>(type) };
        towers.at( type ) = Tower::make( static_cast<TowerType::Type>(type), position, context );
    }

    context.systems->run_group( Setup );

    double constexpr desired_fps { 60. };
    double constexpr desired_loop_length { 1. / desired_fps };
    double loop_length { 0. };

    while ( not context.window->is_closing() ) {
        std::this_thread::sleep_for( std::chrono::duration<double> { (desired_loop_length - loop_length) * 0.99 } );

        double const loop_start { Time::loop_start() };

        glfwPollEvents();
        context.systems->run_group( General );
        context.window->clear();
        context.systems->run_group( Render );
        context.window->render();

        double const loop_end { Time::get_time() };
        loop_length = loop_end - loop_start;
    }

    return 0;
}
