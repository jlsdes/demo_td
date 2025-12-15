#include "config.hpp"
#include "log.hpp"
#include "time.hpp"

#include <fstream>
#include <iostream>
#include <memory>


Log::Log( std::ostream & stream )
    : m_output { stream } {
}

Log * Log::main_log() {
    static std::unique_ptr<Log> log { nullptr };
    if (log == nullptr)
        log = std::make_unique<Log>( std::cout );
    return log.get();
}

void Log::write_type( Type const type ) const {
    std::string const tags[4] {
        "[\033[96m Debug \033[0m]",
        "[\033[97m Info  \033[0m]",
        "[\033[93mWarning\033[0m]",
        "[\033[91m Error \033[0m]",
    };
    m_output << tags[type];
}

void Log::write_time() const {
    auto const precision { m_output.precision() };
    m_output << '[' << std::fixed << std::setprecision( 7 ) << Time::get_time() << ']';
    m_output << precision << std::defaultfloat;
}
