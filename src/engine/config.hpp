#ifndef DEMO_TD_CONFIG_HPP
#define DEMO_TD_CONFIG_HPP

#include "ini_reader.hpp"

#include <filesystem>
#include <stdexcept>


/** Handles the configuration file(s). If any of the key-value pairs are identical (i.e. same section and same key),
 *  then the last read value will be used. These collisions can occur within the same config file, or between different
 *  files. The internal INI reader is not reset between different file reads.
 *
 *  This class is a singleton class; it can be used through the public static functions.
 */
class Config {
    /** Basic constructor and destructor. */
    Config() = default;
    ~Config() = default;

    /** Returns  */
    static Config & get_instance();

public:
    /** Loads a configuration file; must be a file accepted by the IniReader class. */
    static void load_config( std::filesystem::path const & path );

    /** Clears the loaded configuration data, and loads a new configuration file. */
    static void reload_config( std::filesystem::path const & path );

    /** Changes to a new section, such that future get_value() calls can omit this section. If no section exists with
     *  the given name, the current section is not updated. The return value indicates whether an update has happened.
     */
    static bool set_current_section( std::string const & section );

    /** Returns the value of a given key converted to the appropriate type.
     *
     * @tparam Value The expected type of the value; accepted types are: int, unsigned int, long, unsigned long, float,
     *               double, std::string.
     * @param section (optional) The name of the section under which the key-value pair should exist.
     * @param key The key being requested.
     */
    template <typename Value>
    [[nodiscard]] static Value get_value( std::string const & section, std::string const & key );
    template <typename Value>
    [[nodiscard]] static Value get_value( std::string const & key );

private:
    /// The .ini reader object.
    IniReader m_reader;
    /// The full configuration data.
    IniData const * m_data { nullptr };
    /// The section currently being read, used by get_value() if no section is given.
    std::string m_section;
};


// Template implementation(s)

template <typename Value>
Value Config::get_value( std::string const & section, std::string const & key ) {
    Config const & config { get_instance() };
    if ( config.m_data == nullptr )
        throw std::logic_error( "No config files have been read yet." );
    return config.m_data->at( section ).get<Value>( key );
}

template <typename Value>
Value Config::get_value( std::string const & key ) {
    Config const & config { get_instance() };
    if ( config.m_data == nullptr )
        throw std::logic_error( "No config files have been read yet." );
    return config.m_data->at( config.m_section ).get<Value>( key );
}

#endif //DEMO_TD_CONFIG_HPP
