#include "config.hpp"
#include "log.hpp"
#include "time.hpp"

#include <chrono>
#include <fstream>


Log::Log( std::ostream & stream )
    : m_target { std::make_unique<Target>( &stream ) }, m_enable_colours { true } {
    log( "Starting log at (UTC) ", std::chrono::system_clock::now(), "." );
}

Log::Log( std::string const & filename )
    : m_target { std::make_unique<FileTarget>( filename ) }, m_enable_colours { false } {
    log( "Starting log at (UTC) ", std::chrono::system_clock::now(), "." );
}

Log::~Log() {
    log( "Closing log at (UTC) ", std::chrono::system_clock::now(), "." );
}

Log & Log::get_main( std::string const & filename ) {
    static std::unique_ptr<Log> main_log { nullptr };
    if ( main_log == nullptr )
        main_log = filename.empty() ? std::make_unique<Log>( std::cout ) : std::make_unique<Log>( filename );
    return *main_log;
}

void Log::set_colours( bool const enable_colours ) {
    m_enable_colours = enable_colours;
}

void Log::set_output( std::ostream & stream ) {
    // Still not sure if changing the output stream should be allowed; at the very least write a warning to both
    log( Warning, "Changing output stream." );
    m_target = std::make_unique<Target>( &stream );
    log( Warning, "Changed output stream." );
}

void Log::set_output( std::string const & filename ) {
    log( Warning, "Changing output stream to file '", filename, "'." );
    m_target = std::make_unique<FileTarget>( filename );
    log( Warning, "Changed output stream." );
}

void Log::write_type( MessageType const type ) const {
    char constexpr tags[] {
        "[Debug  ]"
        "[Info   ]"
        "[Warning]"
        "[Error  ]"
    };
    unsigned int constexpr tag_size { sizeof(tags) / 4 };
    char constexpr colour_tags[] {
        "[\033[96mDebug  \033[0m]"
        "[\033[97mInfo   \033[0m]"
        "[\033[93mWarning\033[0m]"
        "[\033[91mError  \033[0m]"
    };
    unsigned int constexpr colour_tag_size { sizeof(colour_tags) / 4 };
    if ( m_enable_colours )
        m_target->stream->write( colour_tags + colour_tag_size * type, colour_tag_size );
    else
        m_target->stream->write( tags + tag_size * type, tag_size );
}

void Log::write_time() const {
    auto const precision { m_target->stream->precision() };
    *m_target->stream << '[' << std::fixed << std::setprecision( 7 ) << Time::get_time() << ']';
    *m_target->stream << std::setprecision( static_cast<int>(precision) ) << std::defaultfloat;
}

Log::Target::Target( std::ostream * stream )
    : stream { stream } {
}

Log::FileTarget::FileTarget( std::string const & filename )
    : Target { nullptr }, file { std::make_unique<std::ofstream>( filename ) } {
    stream = file.get();
}

Log::FileTarget::~FileTarget() = default;
