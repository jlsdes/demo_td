#ifndef DEMO_TD_ECS_FACTORY_HPP
#define DEMO_TD_ECS_FACTORY_HPP

#include "entity/entity_manager.hpp"
#include "component/component_manager.hpp"
#include "system/system_manager.hpp"


// Entity-Component-System implementation based on https://austinmorlan.com/posts/entity_component_system/
// but with differences


struct ECS {
    EntityManager entities;
    ComponentManager components;
    SystemManager systems;

    ECS();
};


#endif //DEMO_TD_ECS_FACTORY_HPP