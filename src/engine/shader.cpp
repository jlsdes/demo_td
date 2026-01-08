#include "shader.hpp"
#include "../utils/log.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <fstream>
#include <string>
#include <stdexcept>


Shader::Shader()
    : m_program { glCreateProgram() } {
}

Shader::~Shader() {
    glDeleteProgram( m_program );
}

void Shader::use() const {
    glUseProgram( m_program );
}

void Shader::set_uniform( char const * name, bool const value ) const {
    glUniform1i( get_uniform_location( name ), static_cast<int>(value) );
}

void Shader::set_uniform( char const * name, int const value ) const {
    glUniform1i( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, unsigned int const value ) const {
    glUniform1ui( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, float const value ) const {
    glUniform1f( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, double const value ) const {
    glUniform1d( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, glm::vec3 const & value ) const {
    glUniform3fv( get_uniform_location( name ), 1, glm::value_ptr( value ) );
}

void Shader::set_uniform( char const * name, glm::mat4 const & value ) const {
    glUniformMatrix4fv( get_uniform_location( name ), 1, GL_FALSE, glm::value_ptr( value ) );
}

std::filesystem::path Shader::get_shader_directory() {
    return (std::filesystem::path( __FILE__ ) / "../../shader").lexically_normal();
}

int Shader::get_uniform_location( char const * name ) const {
    use();
    return glGetUniformLocation( m_program, name );
}

GraphicsShader::GraphicsShader( std::string const & vertex_path, std::string const & fragment_path ) {
    unsigned int const vertex_shader { compile_shader( GL_VERTEX_SHADER, vertex_path ) };
    unsigned int const fragment_shader { compile_shader( GL_FRAGMENT_SHADER, fragment_path ) };

    glAttachShader( m_program, vertex_shader );
    glAttachShader( m_program, fragment_shader );
    glLinkProgram( m_program );

    int success;
    char log[1024];
    glGetProgramiv( m_program, GL_LINK_STATUS, &success );
    if ( !success ) {
        glGetProgramInfoLog( m_program, 1024, nullptr, log );
        throw std::runtime_error( std::format( "Failed to link shader program\n{}", log ) );
    }
    Log::info("Shader program built");

    glDeleteShader( fragment_shader );
    glDeleteShader( vertex_shader );
}

unsigned int compile_shader( unsigned int const type, std::string const & filename ) {
    Log::info( "Building shader ", filename );
    // Load the entire shader file into memory
    std::ifstream file { filename };
    std::string const shader_string { std::istreambuf_iterator( file ), std::istreambuf_iterator<char>() };
    char const * const shader_source { shader_string.c_str() };
    file.close();

    // Compile the code
    unsigned int const shader { glCreateShader( type ) };
    glShaderSource( shader, 1, &shader_source, nullptr );
    glCompileShader( shader );

    // Check for errors
    int success;
    char log[1024];
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
    if ( !success ) {
        glGetShaderInfoLog( shader, 1024, nullptr, log );
        throw std::runtime_error( std::format( "Failed to compile shader '{}'.\n{}", filename, log ) );
    }

    return shader;
}
