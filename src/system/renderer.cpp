#include "renderer.hpp"
#include "component/component_manager.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"


Renderer::Renderer( ComponentFlags const flags ) : System { flags } {}

void Renderer::run( EntityManager const & entities, ComponentManager & components ) {
    for ( auto drawable { components.begin<Drawable>() }; drawable != components.end<Drawable>(); ++drawable ) {
        drawable->shader->use();
        drawable->mesh->draw();
    }
}

