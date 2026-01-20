#include "sphere.hpp"
#include "engine/entity_factory.hpp"


Shader * Sphere::s_shader;


void Sphere::initialise( Shader * const shader ) {
    s_shader = shader;

    auto & factory { EntityFactory::get_instance() };
    factory.register_model_factory( "sphere", std::make_unique<Model> );
    factory.register_view_factory( "sphere", get_view_factory<View, Model>() );
    factory.register_controller_factory( "sphere", get_controller_factory<Controller, Model>() );
}

void Sphere::Model::update() {
    auto const old_data { get_old_data() };
    auto const new_data { get_new_data() };
    *new_data = *old_data;
}

glm::vec3 constexpr red { 1.f, 0.f, 0.f };

Sphere::View::View( Model * const model ) : VisibleObject {
    model, Opaque, MeshBuilder::sphere( 10 ).colour( red ).get_mesh(), s_shader
} {}

void Sphere::View::update() {
    auto const data { dynamic_cast<Model *>(m_model)->get_render_data() };
    set_position( data->position );
}

Sphere::Controller::Controller( Model * const model ) : ControllerObject { model } {}

void Sphere::Controller::update() {}
