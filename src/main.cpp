// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "core/camera.hpp"
#include "core/input_manager.hpp"
#include "core/mesh.hpp"
#include "core/shader.hpp"
#include "core/window.hpp"

#include "utils/error.hpp"

#include <filesystem>
#include <print>
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

    auto const shader_dir { ( std::filesystem::path { __FILE__ } / "../shader/" ).lexically_normal() };
    Shader shader { shader_dir / "main.vert", shader_dir / "main.frag" };
    shader.use();

    Camera camera {};
    camera.update_view( shader );
    camera.update_projection( shader );

    shader.set_uniform( "model", glm::identity<glm::mat4>() );

    std::vector<Vertex> const vertices {
        { { -1.f, -1.f, 0.f }, {}, { 255, 0, 0, 255 } },
        { { 1.f, -1.f, 0.f },  {}, { 0, 255, 0, 255 } },
        { { 0.f, 1.f, 0.f },   {}, { 0, 0, 255, 255 } },
    };
    Mesh mesh { vertices };

    InputManager input_manager {};
    input_manager.add_observer(
        Observer { KeyboardObserver { []( int const key, int const scancode, int const action, int const mods ) {
                       std::println( "KEYBOARD ACTION {} {} {} {}", key, scancode, action, mods );
                   } },
                   GLFW_KEY_F } );

    while ( not window.is_closing() ) {
        glfwPollEvents();

        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        mesh.draw();
        window.draw();

        report_errors();
    }

    return 0;
}
