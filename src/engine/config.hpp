#ifndef DEMO_TD_CONFIG_HPP
#define DEMO_TD_CONFIG_HPP

#include "ini_reader.hpp"

#include <filesystem>
#include <stdexcept>


/** Handles the configuration file(s). If any of the key-value pairs are identical (i.e. same section and same key),
 *  then the last read value will be used. These collisions can occur within the same config file, or between different
 *  files. The internal INI reader is not reset between different file reads. */
class Config {
public:
    /** Basic constructor and destructor. */
    Config() = default;
    ~Config() = default;

    /** Loads a configuration file; must be a file accepted by the IniReader class. */
    void load_config( std::filesystem::path const & path );

    /** Returns the value of a given key converted to the appropriate type.
     *
     * @tparam Value The expected type of the value; accepted types are: int, unsigned int, long, unsigned long, float,
     *               double, std::string.
     * @param section The name of the section under which the key-value pair should exist.
     * @param key The key being requested.
     */
    template <typename Value>
    [[nodiscard]] Value get_value( std::string const & section, std::string const & key ) const;

private:
    /// The .ini reader object.
    IniReader m_reader;
    /// The full configuration data.
    IniData const * m_data { nullptr };
};


// Template implementation(s)

template <typename Value>
Value Config::get_value( std::string const & section, std::string const & key ) const {
    if ( m_data == nullptr )
        throw std::logic_error( "No config files have been read yet." );
    return m_data->at( section ).get<Value>( key );
}

#endif //DEMO_TD_CONFIG_HPP
