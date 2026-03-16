#include "shader.hpp"

#include <glad/gl.h>

#include <format>
#include <fstream>
#include <regex>
#include <stdexcept>

#include <print>


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

    ShaderSource result {};
    while ( std::getline( stream, line ) ) {
        std::println( "{} {}", std::regex_match( line, definition_pattern ), line );
        if ( std::regex_match( line, match, definition_pattern ) )
            result.definitions.emplace( match[1], match[2] );
        else
            result.source.emplace_back( line );
    }
    return result;
}

void compile_shader( ShaderSource const & source ) {}

Shader::Shader( std::filesystem::path const & vertex_path, std::filesystem::path const & fragment_path )
        : m_sources {}, m_program { glCreateProgram() } {
    m_sources.push_back( load_shader( vertex_path ) );
    m_sources.push_back( load_shader( fragment_path ) );
}
