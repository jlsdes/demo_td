#include "entity_component_system.hpp"


ECS::ECS() : entities { this }, components { this }, systems { this } {}
