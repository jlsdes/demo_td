#ifndef DEMO_TD_POSITION_HPP
#define DEMO_TD_POSITION_HPP

#include "component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


struct Location : Component {
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 velocity { 0.f }; // TODO Move to separate component
};


#endif //DEMO_TD_POSITION_HPP