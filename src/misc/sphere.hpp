#ifndef DEMO_TD_SPHERE_HPP
#define DEMO_TD_SPHERE_HPP

#include "engine/controller_object.hpp"
#include "engine/mesh_builder.hpp"
#include "engine/model_object.hpp"
#include "engine/view_object.hpp"

#include <glm/glm.hpp>


class Shader;


class Sphere {
public:
    static Shader * s_shader;

    static void initialise( Shader * shader );

    struct Data : public ModelData {
        glm::vec3 position { 0.f };
        float radius { 1.f };
    };

    class Model : public DataModel<Data> {
    public:
        Model() = default;
        ~Model() override = default;

        void update() override;
    };

    class View : public VisibleObject {
    public:
        explicit View( Model * model );
        ~View() override = default;

        void update() override;
    };

    class Controller : public ControllerObject {
    public:
        explicit Controller( Model * model );
        ~Controller() override = default;

        void update() override;
    };
};


#endif //DEMO_TD_SPHERE_HPP
