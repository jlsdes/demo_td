#ifndef DEMO_TD_TOWER_DATA_HPP
#define DEMO_TD_TOWER_DATA_HPP

#include "component.hpp"

#include <glm/glm.hpp>


struct TowerData : Component {

    enum Type : unsigned char {
        Basic,
        Mortar,
        Sniper,
        Multi,
        Melee,
        Status,
        NumberTypes // Valid tower type values go from 0 to NumberTypes
    };

    static constexpr glm::vec3 colours[NumberTypes] {
        glm::vec3 { 0.f, 0.f, 1.f },
        glm::vec3 { 0.f, 1.f, 0.f },
        glm::vec3 { 1.f, 0.f, 0.f },
        glm::vec3 { 1.f, 1.f, 0.f },
        glm::vec3 { 1.f, 0.f, 1.f },
        glm::vec3 { 0.f, 1.f, 1.f },
    };

    Type type;
};


#endif //DEMO_TD_TOWER_DATA_HPP