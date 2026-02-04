#ifndef DEMO_TD_DRAWABLE_HPP
#define DEMO_TD_DRAWABLE_HPP

#include "component.hpp"
#include "graphics/mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Shader;


// Suggestions for drawing priorities with higher values being prioritised over lower values. These values are based on
// which type of elements should typically appear in front of which other types of elements, except for transparent
// objects, which should be drawn last anyway. The renderer uses these values as a baseline but may modify them, e.g. by
// taking distance of the object to the camera into account.
enum PriorityHint : unsigned int {
    Ui = 50,
    Opaque = 40,
    Terrain = 30,
    Skybox = 20,
    Transparent = 10,
};


struct Drawable : Component {
    Mesh<ColourVertex> * mesh { nullptr };
    Shader * shader { nullptr };

    glm::quat rotation {};
    glm::vec3 scale { 1.f };

    PriorityHint priority { Opaque };
};


#endif //DEMO_TD_DRAWABLE_HPP
