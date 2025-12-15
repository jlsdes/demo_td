#ifndef DEMO_TD_LOG_HPP
#define DEMO_TD_LOG_HPP

#include <format>
#include <ostream>
#include <string>


/** Logging utility class. */
class Log {
public:
    /** Constructor and destructor. */
    explicit Log( std::ostream & stream );
    ~Log() = default;;

    /** Deleted copy constructor and assignment operator, to avoid multiple loggers interfering with each other. */
    Log( Log const & ) = delete;
    Log & operator=( Log const & ) = delete;

    /// The types of log message.
    enum Type {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3
    };

    /** Writes a single line to the log, consisting of multiple messages and/or values.
     *
     * @tparam Args The types of the values being logged; these must allow 'std::ostream >> Args'.
     * @param type The type of log message; if omitted 'Info' is used.
     * @param args The values being logged.
     */
    template <typename... Args>
    void log( Type type, Args... args );
    template <typename... Args>
    void log( Args... args );

    /** Writes a single line to the log using std::format().
     *
     * @tparam Args The types of the format parameters.
     * @param type The type of log message; if omitted 'Info' is used.
     * @param message The format string, to be used internally with std::format().
     * @param args The format arguments, to be used internally with std::format().
     */
    template <typename... Args>
    void logf( Type type, std::string const & message, Args const &... args );
    template <typename... Args>
    void logf( std::string const & message, Args const &... args );

    /** Returns a Log object to be used as the main logger. */
    inline static Log * main_log();

private:
    /** Writes a message type to the log; to be used at the start of every log message. */
    void write_type( Type type ) const;

    /** Writes a timestamp to the log. */
    void write_time() const;

private:
    /// The output stream.
    std::ostream & m_output;
};


// Template implementations

template <typename... Args>
void Log::log( Type const type, Args... args ) {
    write_time();
    write_type( type );
    (((m_output << ' ') << std::forward<Args>( args )), ...) << '\n';
}

template <typename... Args>
void Log::log( Args... args ) {
    log( Info, args... );
}

template <typename... Args>
void Log::logf( Type const type, std::string const & message, Args const &... args ) {
    write_time();
    write_type( type );
    m_output << ' ' << std::format( message, args... ) << '\n';
}

template <typename... Args>
void Log::logf( std::string const & message, Args const &... args ) {
    logf( Info, message, args... );
}


#endif //DEMO_TD_LOG_HPP
