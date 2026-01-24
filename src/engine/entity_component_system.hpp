#ifndef DEMO_TD_ECS_FACTORY_HPP
#define DEMO_TD_ECS_FACTORY_HPP

#include "entity/entity_manager.hpp"
#include "component/component_manager.hpp"
#include "system/system_manager.hpp"


struct ECS {
    EntityManager entities;
    ComponentManager components;
    SystemManager systems;

    ECS();
};


#endif //DEMO_TD_ECS_FACTORY_HPP