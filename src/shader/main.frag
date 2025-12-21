#version 460 core

// Interpolated values received from the vertex shader
in vec3 position;
in vec3 normal;
in vec3 colour;

// Uniform values related to lighting
uniform vec3 ambient_light;
uniform vec3 sun_light;
uniform vec3 sun_position; // Should be normalised already

uniform vec3 camera_position;

// The end result: the fragment's colour
out vec4 fragment_colour;


// Some constants, these should become material attributes at some point I think
// No ambient factor; this can be changed by scaling the ambient_light uniform
#define diffuse_factor 1.0f
#define specular_factor 0.5f
#define shininess 32


void main() {
    float diffuse_sun_strength = max( dot( normalize( normal ), sun_position ), 0.0f );
    vec3 sun_diffuse = diffuse_factor * diffuse_sun_strength * sun_light;

    vec3 view_direction = normalize( camera_position - position );
    vec3 reflect_direction = reflect( -sun_position, normal );
    float specular_sun_strength = pow( max( dot( view_direction, reflect_direction ), 0.0f ), shininess );
    vec3 sun_specular = specular_factor * specular_sun_strength * sun_light;

    fragment_colour = vec4( ( ambient_light + sun_diffuse + sun_specular ) * colour, 1.0f );
}