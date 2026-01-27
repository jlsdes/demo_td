#include "shader.hpp"
#include "utils/log.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <format>
#include <fstream>
#include <string>
#include <stdexcept>
#include <utility>


int g_success;
char g_log[1024];


Shader::Shader()
    : m_program { glCreateProgram() } {}

Shader::~Shader() {
    glDeleteProgram( m_program );
}

Shader::Shader( Shader && other ) noexcept : m_program { std::exchange( other.m_program, 0 ) } {}

Shader & Shader::operator=( Shader && other ) noexcept {
    m_program = std::exchange( other.m_program, 0 );
    return *this;
}

void Shader::use() const {
    static unsigned int program_in_use { 0 };
    if ( m_program == program_in_use )
        return;
    glUseProgram( m_program );
    program_in_use = m_program;
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

void Shader::set_uniform( char const * name, glm::vec2 const & value ) const {
    glUniform2fv( get_uniform_location( name ), 1, glm::value_ptr( value ) );
}

void Shader::set_uniform( char const * name, glm::vec3 const & value ) const {
    glUniform3fv( get_uniform_location( name ), 1, glm::value_ptr( value ) );
}

void Shader::set_uniform( char const * name, glm::vec4 const & value ) const {
    glUniform4fv( get_uniform_location( name ), 1, glm::value_ptr( value ) );
}

void Shader::set_uniform( char const * name, glm::mat2 const & value ) const {
    glUniformMatrix2fv( get_uniform_location( name ), 1, GL_FALSE, glm::value_ptr( value ) );
}

void Shader::set_uniform( char const * name, glm::mat3 const & value ) const {
    glUniformMatrix3fv( get_uniform_location( name ), 1, GL_FALSE, glm::value_ptr( value ) );
}

void Shader::set_uniform( char const * name, glm::mat4 const & value ) const {
    glUniformMatrix4fv( get_uniform_location( name ), 1, GL_FALSE, glm::value_ptr( value ) );
}

int Shader::get_uniform_location( char const * name ) const {
    use();
    return glGetUniformLocation( m_program, name );
}

/// Helper function that compiles a single shader source file.
unsigned int compile_shader( unsigned int const type, std::string const & filename ) {
    Log::info( "Building shader ", filename );

    std::ifstream file { filename };
    std::string const shader_string { std::istreambuf_iterator( file ), std::istreambuf_iterator<char>() };
    char const * const shader_source { shader_string.c_str() };
    file.close();

    unsigned int const shader { glCreateShader( type ) };
    glShaderSource( shader, 1, &shader_source, nullptr );
    glCompileShader( shader );

    glGetShaderiv( shader, GL_COMPILE_STATUS, &g_success );
    if ( not g_success ) {
        glGetShaderInfoLog( shader, 1024, nullptr, g_log );
        throw std::runtime_error( std::format( "Failed to compile shader '{}'.\n{}", filename, g_log ) );
    }
    return shader;
}

Shader Shader::build_graphics_shader( std::filesystem::path const & vertex_path,
                                      std::filesystem::path const & fragment_path ) {
    unsigned int const vertex_shader { compile_shader( GL_VERTEX_SHADER, vertex_path ) };
    unsigned int const fragment_shader { compile_shader( GL_FRAGMENT_SHADER, fragment_path ) };

    Shader shader {};
    glAttachShader( shader.m_program, vertex_shader );
    glAttachShader( shader.m_program, fragment_shader );
    glLinkProgram( shader.m_program );

    glGetProgramiv( shader.m_program, GL_LINK_STATUS, &g_success );
    if ( not g_success ) {
        glGetProgramInfoLog( shader.m_program, 1024, nullptr, g_log );
        throw std::runtime_error( std::format( "Failed to link shader program\n{}", g_log ) );
    }
    Log::info( "Shader program built" );

    glDeleteShader( fragment_shader );
    glDeleteShader( vertex_shader );

    return shader;
}

std::pair<unsigned int, Shader &> ShaderStore::insert_shader( Shader && shader ) {
    m_shaders.push_back( std::move( shader ) );
    return { m_shaders.size() - 1, m_shaders.back() };
}

std::pair<unsigned int, Shader &> ShaderStore::emplace_shader( std::filesystem::path const & vertex_path,
                                                               std::filesystem::path const & fragment_path ) {
    return insert_shader( Shader::build_graphics_shader( vertex_path, fragment_path ) );
}

Shader & ShaderStore::get_shader( unsigned int const shader_id ) {
    assert( shader_id < m_shaders.size() );
    return m_shaders.at( shader_id );
}

Shader const * ShaderStore::begin() const {
    return m_shaders.begin().base();
}

Shader const * ShaderStore::end() const {
    return m_shaders.end().base();
}
