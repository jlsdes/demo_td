#include "movement.hpp"

#include "component/component_manager.hpp"
#include "component/location.hpp"
#include "entity/entity_manager.hpp"
#include "utils/time.hpp"


void Movement::run( EntityManager const & entities, ComponentManager & components ) {
    auto const elapsed { static_cast<float>(Time::get_elapsed_time()) };

    for ( auto & location : components.get_array<Location>() ) {
        location.position += glm::vec3 { location.velocity.x, 0.f, location.velocity.y } * elapsed;
    }
}
