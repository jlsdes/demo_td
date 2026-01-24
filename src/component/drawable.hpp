#ifndef DEMO_TD_DRAWABLE_HPP
#define DEMO_TD_DRAWABLE_HPP

#include "component.hpp"
#include "engine/mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Shader;


struct Drawable : Component {
    Mesh<ColourVertex> * mesh {nullptr };
    Shader * shader { nullptr };

    // Other stuff will need the position, so this might need to move to a different component
    glm::vec3 position { 0.f };
    glm::quat rotation { } ;
    glm::vec3 scale { 1.f };
};


#endif //DEMO_TD_DRAWABLE_HPP