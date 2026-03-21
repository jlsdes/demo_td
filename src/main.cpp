#include <lib/gl.hpp>

#include "core/mesh.hpp"
#include "core/shader.hpp"
#include "core/window_context.hpp"

#include "utils/error.hpp"

#include <filesystem>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void initialise_glfw() {
    if ( not glfwInit() )
        throw std::runtime_error( "Failed to initialise GLFW." );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 );
}


int main() {
    initialise_glfw();
    WindowContext context {};

    auto const shader_dir { ( std::filesystem::path { __FILE__ } / "../shader/" ).lexically_normal() };
    Shader shader { shader_dir / "main.vert", shader_dir / "main.frag" };
    shader.use();

    context.camera.update_view( shader );
    context.camera.update_projection( shader );

    shader.set_uniform( "model", glm::identity<glm::mat4>() );

    std::vector<Vertex> const vertices {
        { { -1.f, -1.f, 0.f }, {}, { 255, 0, 0, 255 } },
        { { 1.f, -1.f, 0.f },  {}, { 0, 255, 0, 255 } },
        { { 0.f, 1.f, 0.f },   {}, { 0, 0, 255, 255 } },
    };
    Mesh mesh { vertices };

    while ( not context.window.is_closing() ) {
        glfwPollEvents();

        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        mesh.draw();
        context.window.draw();

        report_errors();
    }

    return 0;
}
