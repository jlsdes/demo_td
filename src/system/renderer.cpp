#include "renderer.hpp"
#include "component/component_manager.hpp"
#include "component/drawable.hpp"
#include "component/position.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>


void Renderer::run( EntityManager const & entities, ComponentManager & components ) {
    ComponentFlags const position_flag { id_to_flag( components.get_type_id<Position>() ) };

    for ( auto iterator { components.begin<Drawable>() }; iterator != components.end<Drawable>(); ++iterator ) {
        EntityID const entity { iterator.get_entity() };
        Drawable & drawable { iterator.get_component() };

        drawable.shader->use();

        // Compute the object's current transformation matrix
        auto transformation { glm::identity<glm::mat4>() };
        if ( entities.has_flags( iterator.get_entity(), position_flag ) ) {
            Position const & position { components.get_component<Position>( entity ) };
            transformation = glm::translate( transformation, position.position );
        }
        transformation = glm::scale( transformation, drawable.scale );
        transformation *= glm::mat4_cast( drawable.rotation );
        drawable.shader->set_uniform( "model", transformation );

        if ( not drawable.mesh->is_initialised() )
            drawable.mesh->initialise_gl_objects();
        drawable.mesh->draw();
    }
}
