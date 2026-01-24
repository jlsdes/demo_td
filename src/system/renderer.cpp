#include "renderer.hpp"
#include "component/component_manager.hpp"


Renderer::Renderer( Coordinator const & coordinator ): System { 1 } {}

void Renderer::run( EntityManager const & entities, ComponentManager & components ) {
    for ( auto & drawable : components.get_array<Drawable>() ) {}
}

