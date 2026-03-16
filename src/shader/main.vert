#version 460 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_colour;

out vec3 inter_position;
out vec3 inter_colour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
    inter_position = vec3( model * vec4( vertex_position, 1 ) );
    inter_colour = vertex_colour;

    gl_Position = ( projection * view * model ) * vec4( vertex_position, 1 );
}
