#version 460 core

in vec3 inter_position;
in vec3 inter_colour;

out vec4 out_colour;


void main() {
    out_colour = vec4( inter_colour, 1 );
}
