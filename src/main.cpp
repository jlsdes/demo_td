#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <print>


bool initialise_glfw() {
    if ( not glfwInit() ) {
        std::println( std::cerr, "Failed to initialise GLFW" );
        return false;
    }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, 8 );
    return true;
}


bool initialise_glad() {
    if ( not gladLoadGL( glfwGetProcAddress ) ) {
        std::println( std::cerr, "Failed to initialise GLAD" );
        return false;
    }
    glClearColor( 0.1, 0.1, 0.1, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    return true;
}


int main() {
    if ( not initialise_glfw() )
        return 1;

    auto const window { glfwCreateWindow( 800, 600, "Demo TD", nullptr, nullptr ) };
    if ( not window ) {
        std::println( std::cerr, "Failed to create window" );
        return 1;
    }
    glfwMakeContextCurrent( window );

    if ( not initialise_glad() )
        return 1;

    glfwSetKeyCallback( window, []( GLFWwindow * const window, int const key, int, int const action, int ) {
        if ( key == GLFW_KEY_ESCAPE and action == GLFW_PRESS )
            glfwSetWindowShouldClose( window, true );
    } );

    while ( not glfwWindowShouldClose( window ) ) {
        glfwPollEvents();
        glfwSwapBuffers( window );
    }

    return 0;
}
