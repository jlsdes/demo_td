#include "sphere.hpp"
#include "engine/mesh_builder.hpp"
#include "engine/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>


Shader * Sphere::s_shader;
glm::vec3 g_colour;

void Sphere::initialise( Shader * const shader ) {
    s_shader = shader;

    auto & factory { EntityFactory::get_instance() };
    factory.register_model_factory( "sphere", std::make_unique<Model> );
    factory.register_view_factory( "sphere", std::make_unique<View> );
    factory.register_controller_factory( "sphere", std::make_unique<Controller> );
}

Entity Sphere::create( glm::vec3 const & position, float const radius, glm::vec3 const & colour ) {
    g_colour = colour;

    Entity sphere { EntityFactory::get_instance().build( "sphere" ) };

    auto const [model_id, model] { sphere.model };
    model->get_model_data<ModelData>()->position = position;

    auto const [view_id, view] { sphere.view };
    view->scale( radius );
    view->translate( position );

    return sphere;
}

Mesh get_coloured_mesh() {
    auto sphere { MeshBuilder::sphere( 10 ) };
    sphere.m_colours = { sphere.m_vertices.size(), g_colour };
    return sphere.get_mesh();
}

Sphere::View::View() : ViewObject { Opaque, std::move( get_coloured_mesh() ), s_shader } {}

void Sphere::View::update() {
    ViewObject::update();
}

Sphere::Controller::Controller() = default;

void Sphere::Controller::update() {}
