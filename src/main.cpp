// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "core/mesh.hpp"
#include "core/shader.hpp"
#include "core/window.hpp"

#include <numbers>
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
    Window window {};

    Shader shader { "./src/shader/main.vert", "./src/shader/main.frag" };

    shader.set_uniform( "model", glm::identity<glm::mat4>() );
    shader.set_uniform( "view", glm::identity<glm::mat4>() );
    shader.set_uniform( "projection", glm::perspective( std::numbers::pi_v<float> / 4.f, 4.f / 3.f, 0.1f, 100.f ) );

    std::vector<Vertex> const vertices {
        { { 0.5f, 0.f, 0.f }, {}, { 0, 255, 0, 255 } },
        { { -0.5f, 0.f, 0.f }, {}, { 255, 0, 0, 255 } },
        { { 0.f, 1.f, 0.f }, {}, { 0, 0, 255, 255 } },
    };
    Mesh mesh { vertices };

    while ( not window.is_closing() ) {
        glfwPollEvents();
        mesh.draw();
        window.draw();
    }

    return 0;
}
