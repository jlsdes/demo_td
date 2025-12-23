#version 460 core

// Input data from the main program
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_colour;

// To be interpolated values sent to the fragment shader
out vec3 position;
out vec3 normal;
out vec3 colour;

// The components of the transformation matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    position = vec3( model * vec4( vertex_position, 1.0 ) );
    normal = vertex_normal;
    colour = vertex_colour;

    mat4 transformation = projection * view * model;
    gl_Position = transformation * vec4( vertex_position, 1.0 );
}