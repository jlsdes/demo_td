#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"
#include "graphics/window.hpp"
#include "graphics/context.hpp"
#include "graphics/entity_component_system.hpp"

#include "entity/tower_factory.hpp"


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

    auto window { context.get_window() };
    while ( not window->is_closing() ) {
        Time::loop_start();
        window->clear();

        glfwPollEvents();
        ecs.systems.run_group( Render );

        window->render();
    }

    return 0;
}
