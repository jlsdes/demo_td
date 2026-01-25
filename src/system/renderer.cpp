#include "renderer.hpp"
#include "component/component_manager.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>


void Renderer::run( EntityManager const & entities, ComponentManager & components ) {
    for ( auto drawable { components.begin<Drawable>() }; drawable != components.end<Drawable>(); ++drawable ) {
        drawable->shader->use();

        auto transformation { glm::identity<glm::mat4>() };
        transformation = glm::translate( transformation, drawable->position );
        transformation = glm::scale( transformation, drawable->scale );
        transformation *= glm::mat4_cast( drawable->rotation );
        drawable->shader->set_uniform( "model", transformation );

        drawable->mesh->draw();
    }
}
