#include "shader.hpp"

#include <glad/gl.h>

#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <regex>
#include <stdexcept>


struct ShaderSource {
    std::string filename;
    unsigned int shader_id;

    std::map<std::string, std::string> definitions;
    std::vector<std::string> source;
};

Shader::Shader() : m_sources {}, m_program { glCreateProgram() } {}

Shader::Shader( std::filesystem::path const & vertex_path, std::filesystem::path const & fragment_path ) : Shader {} {
    add( vertex_path, GL_VERTEX_SHADER );
    add( fragment_path, GL_FRAGMENT_SHADER );
    build();
}

Shader::~Shader() {
    for ( auto const & source : m_sources )
        glDeleteShader( source->shader_id );
    glDeleteProgram( m_program );
}

void Shader::add( std::filesystem::path const & path, unsigned int const shader_type ) {
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

    std::unique_ptr<ShaderSource> result { std::make_unique<ShaderSource>() };
    result->filename = path.string();
    result->shader_id = glCreateShader( shader_type );

    while ( std::getline( stream, line ) ) {
        if ( std::regex_match( line, match, definition_pattern ) )
            result->definitions.emplace( match[1], match[2] );
        else
            result->source.emplace_back( line );
    }
    m_sources.emplace_back( std::move( result ) );
}

void Shader::build() {
    int success;
    char log[512];
    std::string full_source;

    for ( auto const & source : m_sources ) {
        // Reset the source string and ensure that the "#version ..." line is at the top
        full_source = source->source[0];
        for ( auto const & [name, value] : source->definitions )
            full_source += std::format( "\n#define {} {}", name, value );
        for ( auto line { source->source.cbegin() + 1 }; line != source->source.cend(); ++line )
            full_source += '\n' + *line;
        auto const source_data { full_source.data() };

        glShaderSource( source->shader_id, 1, &source_data, nullptr );
        glCompileShader( source->shader_id );

        glGetShaderiv( source->shader_id, GL_COMPILE_STATUS, &success );
        if ( not success ) {
            glGetShaderInfoLog( source->shader_id, 512, nullptr, log );
            throw std::runtime_error( std::format( "Failed to compile shader {}:\n{}", source->filename, log ) );
        }
        glAttachShader( m_program, source->shader_id );
    }
    glLinkProgram( m_program );

    glGetProgramiv( m_program, GL_LINK_STATUS, &success );
    if ( not success ) {
        glGetProgramInfoLog( m_program, 512, nullptr, log );
        throw std::runtime_error( std::format( "Failed to compile shader program:\n{}", log ) );
    }
}

bool Shader::has_definition( std::string const & name ) const {
    for ( auto const & source : m_sources ) {
        if ( source->definitions.contains( name ) )
            return true;
    }
    return false;
}

std::string Shader::get_definition( std::string const & name ) const {
    for ( auto const & source : m_sources ) {
        if ( source->definitions.contains( name ) )
            return source->definitions.at( name );
    }
    throw std::out_of_range( std::format( "Definition {} could not be found.", name ) );
}

void Shader::set_definition( std::string const & name, std::string const & value ) {
    for ( auto const & source : m_sources ) {
    }
}
