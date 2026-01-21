#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/config.hpp"
#include "utils/log.hpp"

#include "engine/component_manager.hpp"


struct Position : Component {
    Position() = default;
    Position( float const x, float const y, float const z ) : x { x }, y { y }, z { z } {}

    float x;
    float y;
    float z;
};


int main() {
    // auto const main_dir { get_main_dir() };
    // Config::load_config( main_dir / "config.ini" );
    // Log::info( "Loaded config ", (main_dir / "config.ini").string() );
    // glfwTerminate();

    ComponentManager manager {};

    manager.create_store<Position>();
    auto & array { manager.get_component_array<Position>() };
    array.insert( 1 );
    array.insert( 2 );
    array.insert( 3, { 1.f, 2.f, 3.f } );
    for ( auto const pos : array ) {
        Log::debug( pos.x, pos.y, pos.z );
    }
    auto const t { array.get( 3 ) };
    array.remove( 1 );

    Log::debug( manager.get_component_flag<Position>() );
    auto const s { manager.get_component<Position>( 3 ) };

    manager.remove_store<Position>();

    return 0;
}
