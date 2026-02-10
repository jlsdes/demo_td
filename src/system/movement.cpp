#include "movement.hpp"

#include "component/location.hpp"
#include "graphics/entity_component_system.hpp"
#include "utils/time.hpp"


Movement::Movement( ECS * const ecs ) : System { ecs } {}

void Movement::run() {
    auto const elapsed { static_cast<float>(Time::get_elapsed_time()) };

    for ( auto & location : m_ecs->components.get_array<Location>() )
        location.position += location.velocity * elapsed;
}
