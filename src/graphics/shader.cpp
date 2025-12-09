#include "shader.hpp"

#include <glad/gl.h>

#include <format>
#include <fstream>
#include <string>
#include <stdexcept>


Shader::Shader()
    : m_program { glCreateProgram() }
{
}

Shader::~Shader()
{
    glDeleteProgram( m_program );
}

void Shader::use() const
{
    glUseProgram( m_program );
}

void Shader::set_uniform( char const * name, bool const value ) const
{
    glUniform1i( get_uniform_location( name ), static_cast<int>( value ) );
}

void Shader::set_uniform( char const * name, int const value ) const
{
    glUniform1i( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, unsigned int const value ) const
{
    glUniform1ui( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, float const value ) const
{
    glUniform1f( get_uniform_location( name ), value );
}

void Shader::set_uniform( char const * name, double const value ) const
{
    glUniform1d( get_uniform_location( name ), value );
}

std::filesystem::path Shader::get_shader_directory()
{
    return (std::filesystem::path(__FILE__) / "../../shader").lexically_normal();
}

int Shader::get_uniform_location( char const * name ) const
{
    use();
    return glGetUniformLocation( m_program, name );
}

GraphicsShader::GraphicsShader( char const * vertex_path, char const * fragment_path )
    : Shader {}
{
    unsigned int const vertex_shader { compile_shader( GL_VERTEX_SHADER, vertex_path ) };
    unsigned int const fragment_shader { compile_shader( GL_FRAGMENT_SHADER, fragment_path ) };

    // Link these shaders to the shader program
    glAttachShader( m_program, vertex_shader );
    glAttachShader( m_program, fragment_shader );
    glLinkProgram( m_program );

    // Check for errors
    int success;
    char log[1024];
    glGetProgramiv( m_program, GL_LINK_STATUS, &success );
    if ( !success ) {
        glGetProgramInfoLog( m_program, 1024, nullptr, log );
        throw std::runtime_error( std::format( "Failed to link shader program\n{}", log ) );
    }

    glDeleteShader( fragment_shader );
    glDeleteShader( vertex_shader );
}

unsigned int compile_shader( unsigned int const type, char const * const file_name )
{
    // Load the entire shader file into memory
    std::ifstream file { file_name };
    std::string const shader_string { std::istreambuf_iterator<char>( file ), std::istreambuf_iterator<char>() };
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
        throw std::runtime_error( std::format( "Failed to compile shader '{}'.\n{}", file_name, log ) );
    }

    return shader;
}
