#include "movement.hpp"

#include "component/component_manager.hpp"
#include "component/location.hpp"
#include "utils/time.hpp"


Movement::Movement( Context const & context ) : System { context } {}

void Movement::run() {
    auto const elapsed { static_cast<float>(Time::get_elapsed_time()) };

    for ( auto & location : m_context.components->get_array<Location>() )
        location.position += location.velocity * elapsed;
}
