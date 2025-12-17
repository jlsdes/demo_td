#version 460 core

// Interpolated values received from the vertex shader
in vec3 normal;
in vec3 colour;

// The end result: the fragment's colour
out vec4 fragment_colour;

void main()
{
    fragment_colour = vec4( colour, 1.0f );
}