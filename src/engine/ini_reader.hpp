#ifndef DEMO_TD_INI_READER_HPP
#define DEMO_TD_INI_READER_HPP


#include <istream>
#include <map>
#include <string>


/** Contains the data of a single section. */
struct IniSection {
    std::string name;
    std::map<std::string, std::string> data;

    /** Returns the value of a key-value pair as a specific type. Throws an exception if the value cannot be converted,
     *  or if the key isn't present. */
    template <typename Value>
    [[nodiscard]] Value get( std::string const & key ) const;
};

// The IniSection::get() function will be available for the following types by default
template <>
[[nodiscard]] int IniSection::get<int>( std::string const & key ) const;
template <>
[[nodiscard]] unsigned int IniSection::get<unsigned int>( std::string const & key ) const;
template <>
[[nodiscard]] long IniSection::get<long>( std::string const & key ) const;
template <>
[[nodiscard]] unsigned long IniSection::get<unsigned long>( std::string const & key ) const;
template <>
[[nodiscard]] float IniSection::get<float>( std::string const & key ) const;
template <>
[[nodiscard]] double IniSection::get<double>( std::string const & key ) const;
template <>
[[nodiscard]] std::string IniSection::get<std::string>( std::string const & key ) const;
template <>
[[nodiscard]] char const * IniSection::get<char const *>( std::string const & key ) const;


/// The data structure used for storing (a) .ini file(s).
using IniData = std::map<std::string, IniSection>;


/** Reads .ini files and provides access to the contents through maps. Multiple files can be read, but their data will
 *  be merged together if sections with equal names exist, unless the reader is reset inbetween.
 *
 * Some restrictions are placed on the files for this reader to accept them:
 * - the only delimiter allowed is '=', optionally surrounded by spaces on either side.
 * - keys must only contain letters, numbers, and underscores.
 * - key-value pairs before any sections have been started are allowed, and will be stored under the "" section.
 */
class IniReader {
public:
    /** Basic constructor and destructor. */
    IniReader() = default;
    ~IniReader() = default;

    /** Reads the contents of the stream and parses it as a .ini file. */
    IniData const & read( std::istream & stream );

    /** Returns the data of the entire section as a string-to-string map. No type conversion will be performed by this
     *  function. */
    [[nodiscard]] IniSection const & get_section( std::string const & section ) const;

    /** Adds a new section to the reader's data. If a section with the same name already exists, then the two will be
     *  merged. In the case of any conflicts, the new section will win. Empty sections, without any data will be
     *  ignored. */
    void add_section( IniSection const & section );

    /** Clears the data of the reader, so that future files won't have their sections merged with past files. */
    void reset();

private:
    /// The contents of the files that have been read, grouped per section.
    IniData m_data;
};


#endif //DEMO_TD_INI_READER_HPP
