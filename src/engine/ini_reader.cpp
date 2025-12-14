#include "ini_reader.hpp"

#include <format>
#include <functional>
#include <limits>
#include <stdexcept>


auto constexpr max_read_size { std::numeric_limits<std::streamsize>::max() };


template <>
int IniSection::get<int>( std::string const & key ) const {
    return std::stoi( data.at( key ) );
}

template <>
unsigned int IniSection::get<unsigned int>( std::string const & key ) const {
    return static_cast<unsigned int>(std::stoul( data.at( key ) ));
}

template <>
long IniSection::get<long>( std::string const & key ) const {
    // Relying on .at() and stol() to throw exceptions if the key cannot be found or the value not converted
    return std::stol( data.at( key ) );
}

template <>
unsigned long IniSection::get<unsigned long>( std::string const & key ) const {
    return std::stoul( data.at( key ) );
}

template <>
float IniSection::get<float>( std::string const & key ) const {
    return std::stof( data.at( key ) );
}

template <>
double IniSection::get<double>( std::string const & key ) const {
    return std::stod( data.at( key ) );
}

template <>
std::string IniSection::get<std::string>( std::string const & key ) const {
    return data.at( key );
}

template <>
char const * IniSection::get<char const *>( std::string const & key ) const {
    return data.at( key ).c_str();
}

/** Helper function for IniReader::read(); initialises a new IniSection object, and adds the old one to the IniReader's
 *  data if necessary. */
void start_new_section( std::istream & stream, IniReader & reader, IniSection & current_section ) {
    // Does the old section actually contain any data? The initial empty-string-section often does not
    if ( !current_section.data.empty() )
        reader.add_section( current_section );

    current_section = IniSection {};
    // std::getline() extracts the delimiter, but does not append it to the string object
    std::getline( stream, current_section.name, ']' );

    char c;
    stream.get( c );
    // Sections can technically occur on the final line, although this isn't very useful
    if ( !stream.eof() && c != '\n' )
        throw std::invalid_argument( std::format( ".ini config contains character '{}' after section ({}) declaration",
                                                  c, current_section.name ) );
    // TODO Find/create more fitting exceptions
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
    for ( auto const c : key )
        if ( !(c == '_' || std::isalnum( c )) )
            throw std::invalid_argument( std::format( ".ini config contains invalid character '{}' in a key \"{}\"",
                                                      c, key ) );
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
    if ( single_quotes || double_quotes )
        value = value.substr( 1, value.size() - 1 );
    return value;
}

/// Helper for IniReader::read()
void read_key_value( std::istream & stream, IniSection & current_section ) {
    skip_whitespace( stream );
    std::string const key { read_key( stream ) };
    skip_whitespace( stream );
    std::string const value { read_value( stream ) };
    current_section.data.emplace( key, value );
}

IniData const & IniReader::read( std::istream & stream ) {
    IniSection current_section { "", {} };
    char c;
    while ( stream >> c ) {
        switch ( c ) {
        case '\n': // Empty line, just skip
            break;
        case ';': // This is a comment, skip everything until the next line
            stream.ignore( max_read_size, '\n' );
            break;
        case '[': // Start of a new section
            start_new_section( stream, *this, current_section );
            break;
        default: // Key-value pair
            stream.putback( c );
            read_key_value( stream, current_section );
            break;
        }
    }
    add_section( current_section );
    return m_data;
}

IniSection const & IniReader::get_section( std::string const & section ) const {
    return m_data.at( section );
}

void IniReader::add_section( IniSection const & section ) {
    if ( section.data.empty() )
        return;
    if ( m_data.contains( section.name ) ) {
        // Aggressively overwrite any existing data in case of data collisions; map.merge() does not overwrite any data
        for ( auto const & [key, value] : section.data )
            m_data.at( section.name ).data[key] = value;
    } else {
        m_data.emplace( section.name, section );
    }
}

void IniReader::reset() {
    m_data.clear();
}
