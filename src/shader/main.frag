#version 460 core

// Interpolated values received from the vertex shader
in vec3 position;
in vec3 normal;
in vec3 colour;

// Uniform values related to lighting
uniform vec3 ambient_light;
uniform vec3 sun_light;
uniform vec3 sun_direction;
uniform vec3 camera_position;

uniform bool is_light_source;

// The end result: the fragment's colour
out vec4 fragment_colour;


// Some constants, these should become material attributes at some point
// No ambient factor; this can be changed by scaling the ambient_light uniform
#define diffuse_factor 0.2f
#define specular_factor 0.01f
#define shininess 32


/// Returns the diffuse colour contribution of a given light source on the current fragment. This function requires the
/// direction the light source is in relative to the fragment, and the colour of the light.
vec3 diffuse_colour( vec3 light_direction, vec3 light_colour ) {
    float diffuse_strength = max( dot( normalize( normal ), light_direction ), 0.0 );
    return diffuse_factor * diffuse_strength * light_colour;
}


/// Returns the specular colour contribution of a given light source on the current fragment. This function requires the
/// direction the light source is in relative to the fragment, and the colour of the light.
vec3 specular_colour( vec3 light_direction, vec3 light_colour ) {
    vec3 view_direction = normalize( camera_position - position );
    vec3 reflect_direction = reflect( -light_direction, normalize( normal ) );
    float specular_sun_strength = pow( max( dot( view_direction, reflect_direction ), 0.0f ), shininess );
    return specular_factor * specular_sun_strength * sun_light;
}


void main() {
    vec3 sun_diffuse = diffuse_colour( sun_direction, sun_light );
    vec3 sun_specular = specular_colour( sun_direction, sun_light );
    vec3 result = ( ambient_light + sun_diffuse + sun_specular ) * colour;

    if ( is_light_source )
        result = colour;

    // Gamma correction
    result = pow( result, vec3( 1.f / 2.2f ) );

    fragment_colour = vec4( result, 1.0f );
}