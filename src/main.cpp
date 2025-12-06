#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/shader.hpp"
#include "graphics/window.hpp"

#include <filesystem>


int main()
{
    Window const window { 600, 480, "Demo TD" };

    // Find and build the main graphics shader
    auto const shader_path { (std::filesystem::path( __FILE__ ) / "../shader").lexically_normal() };
    GraphicsShader const shader { (shader_path / "main.vert").c_str(), (shader_path / "main.frag").c_str() };
    shader.use();

    // Main program loop
    while ( !window.is_closing() ) {
        window.render();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
