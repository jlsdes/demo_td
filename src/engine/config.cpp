#include "config.hpp"

#include <fstream>
#include <format>
#include <stdexcept>


void Config::load_config( std::filesystem::path const & path ) {
    if ( !path.has_filename() )
        throw std::invalid_argument( std::format( "Path '{}' does not point to a file.", path.string() ) );
    std::ifstream stream { path };
    if ( !stream.is_open() )
        throw std::invalid_argument( std::format( "Unable to open config file '{}'.", path.string() ) );
    m_data = &m_reader.read( stream );
}
