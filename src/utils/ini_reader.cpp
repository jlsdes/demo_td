#include "ini_reader.hpp"
#include "log.hpp"

#include <format>
#include <fstream>
#include <functional>
#include <limits>
#include <stdexcept>


auto constexpr g_max_read_size { std::numeric_limits<std::streamsize>::max() };


template <>
int IniSection::get<int>( std::string const & key ) const {
    return std::stoi( data.at( key ).first );
}

template <>
unsigned int IniSection::get<unsigned int>( std::string const & key ) const {
    return static_cast<unsigned int>(std::stoul( data.at( key ).first ));
}

template <>
long IniSection::get<long>( std::string const & key ) const {
    // Relying on .at() and stol() to throw exceptions if the key cannot be found or the value not converted
    return std::stol( data.at( key ).first );
}

template <>
unsigned long IniSection::get<unsigned long>( std::string const & key ) const {
    return std::stoul( data.at( key ).first );
}

template <>
float IniSection::get<float>( std::string const & key ) const {
    return std::stof( data.at( key ).first );
}

template <>
double IniSection::get<double>( std::string const & key ) const {
    return std::stod( data.at( key ).first );
}

template <>
std::string IniSection::get<std::string>( std::string const & key ) const {
    return data.at( key ).first;
}

template <>
char const * IniSection::get<char const *>( std::string const & key ) const {
    return data.at( key ).first.c_str();
}

template <>
[[nodiscard]] std::filesystem::path IniSection::get<std::filesystem::path>( std::string const & key ) const {
    // Assuming that the source is a valid filename
    std::filesystem::path const source_file { data.at( key ).second };
    std::filesystem::path const source_directory { source_file.parent_path() };
    std::filesystem::path const result { source_directory / data.at( key ).first };
    return result.lexically_normal();
}

/** Helper function for IniReader::read(); initialises a new IniSection object, and adds the old one to the IniReader's
 *  data if necessary. */
void start_new_section( std::istream & stream, IniReader & reader, IniSection & current_section ) {
    // Does the old section actually contain any data? The initial empty-string-section often does not
    if ( not current_section.data.empty() )
        reader.add_section( current_section );

    current_section = IniSection {};
    // std::getline() extracts the delimiter, but does not append it to the string object
    std::getline( stream, current_section.name, ']' );

    char c;
    stream.get( c );
    // Sections can technically occur on the final line, although this isn't very useful
    if ( not stream.eof() and c != '\n' ) {
        Log::warning( ".ini config contains character(s) after section (", current_section.name, ") declaration." );
        stream.ignore( g_max_read_size, '\n' );
    }
}

/// Helper for IniReader::read()
void skip_whitespace( std::istream & stream ) {
    while ( std::isblank( stream.peek() ) )
        stream.get();
}

/// Helper for IniReader::read()
void remove_trailing_whitespace( std::string & text ) {
    while ( !text.empty() && std::isblank( text.at( text.size() - 1 ) ) )
        text.pop_back();
}

/// Helper for IniReader::read()
std::string read_key( std::istream & stream ) {
    std::string key;
    std::getline( stream, key, '=' );
    remove_trailing_whitespace( key );

    // Verify the key; only letters, numbers, and underscores allowed
    if ( key.empty() )
        Log::error( ".ini config contains a line with an empty key, skipping line." );

    unsigned int nr_verified { 0 };
    for ( auto const c : key ) {
        if ( c == '\n' ) {
            Log::error( ".ini config contains an ill-formed line \"", key.substr( 0, nr_verified ),
                        "\", skipping line." );
            key.erase( key.cbegin(), key.cbegin() + nr_verified + 1 );
            nr_verified = 0;
            continue;
        }
        if ( !(c == '_' || std::isalnum( c )) ) {
            Log::error( ".ini config contains invalid character '", c, "' in the key \"", key, "\", skipping line." );
            return "";
        }
        ++nr_verified;
    }
    return key;
}

/// Helper for IniReader::read()
std::string read_value( std::istream & stream ) {
    std::string value;
    std::getline( stream, value, '\n' );
    remove_trailing_whitespace( value );

    // Check whether the value starts and ends with quotes
    bool const single_quotes { value.starts_with( '\'' ) && value.ends_with( '\'' ) };
    bool const double_quotes { value.starts_with( '"' ) && value.ends_with( '"' ) };
    if ( single_quotes or double_quotes )
        value = value.substr( 1, value.size() - 2 );
    return value;
}

/// Helper for IniReader::read()
void read_key_value( std::istream & stream, IniSection & current_section, std::string const & source ) {
    skip_whitespace( stream );

    auto const key { read_key( stream ) };
    if ( key.empty() ) {
        // If the key is invalid, skip the rest of the line
        stream.ignore( g_max_read_size, '\n' );
        return;
    }
    skip_whitespace( stream );
    std::string const value { read_value( stream ) };

    current_section.data.emplace( key, std::pair { value, source } );
}

IniData const & IniReader::read( std::istream & stream, std::string const & source ) {
    m_source = source;
    IniSection current_section { "", {} };
    char c;
    while ( stream >> c ) {
        switch ( c ) {
        case '\n': // Empty line, just skip
            break;
        case ';': // This is a comment, skip everything until the next line
            stream.ignore( g_max_read_size, '\n' );
            break;
        case '[': // Start of a new section
            start_new_section( stream, *this, current_section );
            break;
        default: // Key-value pair
            stream.putback( c );
            read_key_value( stream, current_section, m_source );
            break;
        }
    }
    add_section( current_section );
    m_source = "";
    return m_data;
}

IniData const & IniReader::read( std::filesystem::path const & path ) {
    if ( not path.has_filename() )
        throw std::invalid_argument( std::format( "Path '{}' does not point to a file.", path.string() ) );
    std::ifstream file { path };
    if ( not file.is_open() )
        throw std::invalid_argument( std::format( "Unable to open file '{}'.", path.string() ) );

    // Added 'this->' because otherwise my IDE thinks this can be made static
    return this->read( file, path.generic_string() );
}

IniSection const & IniReader::get_section( std::string const & section ) const {
    return m_data.at( section );
}

void IniReader::add_section( IniSection const & section ) {
    if ( section.data.empty() )
        return;
    if ( m_data.contains( section.name ) ) {
        // Overwrite any existing data in case of data collisions; map.merge() does not overwrite any data
        for ( auto const & [key, value] : section.data )
            m_data.at( section.name ).data[key] = value;
    } else {
        m_data.emplace( section.name, section );
    }
}

void IniReader::reset() {
    m_data.clear();
}
