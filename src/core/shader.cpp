#include "shader.hpp"

#include <glad/gl.h>

#include <format>
#include <fstream>
#include <regex>
#include <stdexcept>


ShaderSource load_shader( std::filesystem::path const & path ) {
    if ( not std::filesystem::exists( path ) )
        throw std::invalid_argument( std::format( "File {} does not exist.", path.string() ) );

    std::ifstream stream { path };
    std::string line;

    // Pattern to match lines that start with "#define", followed by some whitespace ("\s+"),
    // then the name of the definition ("[a-zA-Z_]\w*"), followed by more whitespace, and finally everything else.
    std::regex const definition_pattern { "^#define\\s+([a-zA-Z_]\\w*)\\s+(\\S.*)$" };
    // The two sub-patterns in the pattern above (inside the parentheses) can be extracted by std::regex_match into the
    // std::smatch object below, if the match succeeds. The first element in this resulting object (match[0]) will be
    // the entire matching string, and then match[1] and match[2] will contain the name and value of the definition
    // respectively.
    std::smatch match {};

    ShaderSource result { .filename = path.string() };
    while ( std::getline( stream, line ) ) {
        if ( std::regex_match( line, match, definition_pattern ) )
            result.definitions.emplace( match[1], match[2] );
        else
            result.source.emplace_back( line );
    }
    return result;
}

unsigned int compile_shader( ShaderSource const & source, GLenum const shader_type ) {
    unsigned int shader_id { glCreateShader( shader_type ) };

    std::string full_source { source.source[0] }; // Ensure that the #version line is at the top
    for ( auto const & [name, value] : source.definitions )
        full_source += std::format( "\n#define {} {}", name, value );
    for ( auto line { source.source.cbegin() + 1 }; line != source.source.cend(); ++line )
        full_source += '\n' + *line;
    auto const full_c { full_source.c_str() };

    glShaderSource( shader_id, 1, &full_c, nullptr );
    glCompileShader( shader_id );

    int success;
    char log[512];
    glGetShaderiv( shader_id, GL_COMPILE_STATUS, &success );
    if ( not success ) {
        glGetShaderInfoLog( shader_id, 512, nullptr, log );
        throw std::runtime_error( std::format( "Failed to compile shader {}:\n{}", source.filename, log ) );
    }

    return shader_id;
}

Shader::Shader( std::filesystem::path const & vertex_path, std::filesystem::path const & fragment_path )
        : m_sources {}, m_program { glCreateProgram() } {
    m_sources.push_back( load_shader( vertex_path ) );
    m_sources.push_back( load_shader( fragment_path ) );

    unsigned int const vertex_id { compile_shader( m_sources[0], GL_VERTEX_SHADER ) };
    unsigned int const fragment_id { compile_shader( m_sources[1], GL_FRAGMENT_SHADER ) };

    glAttachShader( m_program, vertex_id );
    glAttachShader( m_program, fragment_id );
    glLinkProgram( m_program );

    int success;
    char log[512];
    glGetProgramiv( m_program, GL_LINK_STATUS, &success );
    if ( not success ) {
        glGetProgramInfoLog( m_program, 512, nullptr, log );
        throw std::runtime_error( std::format( "Failed to compile shader program:\n{}", log ) );
    }
}
