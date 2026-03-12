#include "entity_component_system.hpp"


ECS::ECS( Context & context ) : entities {}, components { context }, systems {} {}
