#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"

#include <filesystem>
#include <iostream>


int main()
{
    {
        Window const window { 600, 480, "Demo TD" };

        // Find and build the main graphics shader
        auto const shader_dir { (std::filesystem::path( __FILE__ ) / "../shader").lexically_normal() };
        GraphicsShader const shader { (shader_dir / "main.vert").c_str(), (shader_dir / "main.frag").c_str() };
        shader.use();

        // Create a simple mesh
        float vertices[] {
            -0.5f, -0.5f, 0.f,
            0.f, 0.5f, 0.f,
            0.5f, -0.5f, 0.f
        };
        Mesh const triangle { vertices, 3 };

        // Main program loop
        while ( !window.is_closing() ) {
            glfwPollEvents();

            triangle.draw();

            window.render();
        }
    }
    glfwTerminate();
    return 0;
}
