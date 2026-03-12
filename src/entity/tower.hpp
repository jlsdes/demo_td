#ifndef DEMO_TD_TOWER_HPP
#define DEMO_TD_TOWER_HPP

#include "entity.hpp"
#include "component/tower_data.hpp"

#include <glm/glm.hpp>


struct ECS;


class Tower {
public:
    [[nodiscard]] static EntityID make( TowerType::Type type, glm::vec3 const & position, ECS * ecs );
};


#endif //DEMO_TD_TOWER_HPP