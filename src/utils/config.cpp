#include "config.hpp"

#include <fstream>
#include <format>
#include <stdexcept>


std::filesystem::path get_main_dir() {
    return (std::filesystem::path( __FILE__ ) / "../../../").lexically_normal();
}

Config & Config::get_instance() {
    static Config config;
    return config;
}

void Config::load_config( std::filesystem::path const & path ) {
    if ( !path.has_filename() )
        throw std::invalid_argument( std::format( "Path '{}' does not point to a file.", path.string() ) );
    std::ifstream stream { path };
    if ( !stream.is_open() )
        throw std::invalid_argument( std::format( "Unable to open config file '{}'.", path.string() ) );

    Config & config { get_instance() };
    config.m_data = &config.m_reader.read( stream );
}

void Config::reload_config( std::filesystem::path const & path ) {
    Config & config { get_instance() };
    config.m_reader.reset();
    load_config( path );
}

bool Config::set_current_section( std::string const & section ) {
    Config & config { get_instance() };
    bool const section_exists { config.m_data->contains( section ) };
    if ( section_exists )
        config.m_section = section;
    return section_exists;
}
