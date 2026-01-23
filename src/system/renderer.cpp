#include "renderer.hpp"

#include "engine/coordinator.hpp"


Renderer::Renderer( Coordinator const & coordinator ): System { coordinator.get_component_flag<Drawable>(), nullptr } {}

void Renderer::run() {}
