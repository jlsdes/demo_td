#ifndef DEMO_TD_DRAWABLE_HPP
#define DEMO_TD_DRAWABLE_HPP

#include "component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Mesh;
class Shader;


struct Drawable : public Component {
    Mesh * mesh {nullptr };
    Shader * shader { nullptr };

    // Other stuff will need the position, so this might need to move to a different component
    glm::vec3 position { 0.f };
    glm::quat rotation { } ;
    glm::vec3 scale { 1.f };
};


#endif //DEMO_TD_DRAWABLE_HPP