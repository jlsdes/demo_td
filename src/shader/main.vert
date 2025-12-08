#version 460 core

layout (location = 0) in vec3 position;

uniform float time;

void main()
{
    vec3 movement = vec3( 0., 0.5 * sin(time), 0. );
    mat3 rotation = mat3(
        cos(time), -sin(time), 0,
        sin(time), cos(time), 0,
        0, 0, 0
    );
    gl_Position = vec4( rotation * position + movement, 1.0 );
}