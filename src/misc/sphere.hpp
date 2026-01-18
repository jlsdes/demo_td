#ifndef DEMO_TD_SPHERE_HPP
#define DEMO_TD_SPHERE_HPP

#include "engine/entity_factory.hpp"


class Shader;


/** Quickly hacked together entity thingy, just to try things out. */
class Sphere {
public:
    static void initialise( Shader * shader );
    static Entity create( glm::vec3 const & position, float radius, glm::vec3 const & colour );

    using Model = ModelObject;

    class View : public ViewObject {
    public:
        explicit View( Model * model );

        void update() override;
    };

    class Controller : public ControllerObject {
    public:
        explicit Controller( Model * model );

        void update() override;
    };

    static Shader * s_shader;
};


#endif //DEMO_TD_SPHERE_HPP
