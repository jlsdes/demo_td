#version 460 core

// Input data from the main program
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_colour;

// To be interpolated values sent to the fragment shader
out vec3 normal;
out vec3 colour;

// The components of the transformation matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    normal = vertex_normal;
    colour = vertex_colour;

    gl_Position = projection * view * model * vec4( position, 1.0 );
}