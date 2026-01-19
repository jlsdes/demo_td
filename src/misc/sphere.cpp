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
    factory.register_view_factory( "sphere", []( ModelObject * const model ) {
        return std::make_unique<View>( dynamic_cast<Model *>(model) );
    } );
    factory.register_controller_factory( "sphere", []( ModelObject * const model ) {
        return std::make_unique<Controller>( dynamic_cast<Model *>(model) );
    } );
}

Entity Sphere::create( glm::vec3 const & position, float const radius, glm::vec3 const & colour ) {
    g_colour = colour;

    Entity sphere { EntityFactory::get_instance().build( "sphere" ) };

    auto const [model_id, model] { sphere.model };
    model->get_new_data<Data>()->position = position;

    auto const [view_id, view] { sphere.view };
    view->translate( position );
    view->scale( radius );

    return sphere;
}

Sphere::Model::Model() : ModelObject {}, m_data { std::make_unique<Data[]>( 3 ) } {
    initialise_data( { m_data.get(), m_data.get() + 1, m_data.get() + 2 } );
}

void Sphere::Model::update() {
    ModelObject::update();
}

Mesh<ColourVertex> get_coloured_mesh() {
    auto sphere { MeshBuilder::sphere( 10 ) };
    sphere.m_colours = { sphere.m_vertices.size(), g_colour };
    return sphere.get_mesh();
}

Sphere::View::View( Model * const model ) : ViewObject { model, Opaque, std::move( get_coloured_mesh() ), s_shader } {}

void Sphere::View::update() {
    float radius;
    {
        auto data { m_model->get_render_data() };
    }

    ViewObject::update();
}

Sphere::Controller::Controller( Model * const model ) : ControllerObject { model } {}

void Sphere::Controller::update() {}
