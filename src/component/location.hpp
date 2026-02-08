#ifndef DEMO_TD_POSITION_HPP
#define DEMO_TD_POSITION_HPP

#include "component.hpp"

#include <glm/glm.hpp>


struct Location : Component {
    glm::vec3 position;
    glm::vec2 velocity;
};


#endif //DEMO_TD_POSITION_HPP