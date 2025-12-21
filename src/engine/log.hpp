#ifndef DEMO_TD_LOG_HPP
#define DEMO_TD_LOG_HPP

#include <format>
#include <iostream>
#include <memory>
#include <string>


/** Logging utility class. */
class Log {
public:
    /** Constructors and destructor. Each Log object can write to either an existing output stream, or a file. If an
     *  output stream is passed to the Log, then it must be kept open while the Log exists. If a filename is given, then
     *  the Log will maintain its own output stream. */
    explicit Log( std::ostream & stream = std::cout );
    explicit Log( std::string const & filename );
    ~Log();;

    /** Deleted copy constructor and assignment operator, to avoid multiple loggers interfering with each other. */
    Log( Log const & ) = delete;
    Log & operator=( Log const & ) = delete;

    /** Moving Log objects shouldn't do anything special. */
    Log( Log && ) = default;
    Log & operator=( Log && ) = default;

    /// The types of log message.
    enum MessageType {
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
    void log( MessageType type, Args... args );
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
    void logf( MessageType type, std::string const & message, Args const &... args );
    template <typename... Args>
    void logf( std::string const & message, Args const &... args );

    /** Sets the toggle for coloured tag types. */
    void set_colours( bool enable_colours );

    /** Changes the output stream of the Log. */
    void set_output( std::ostream & stream );
    void set_output( std::string const & filename );

    /** Returns the same Log object every time to be used as the main log of the program. */
    static Log & get_main( std::string const & filename = "" );

    /** Utility functions for writing to the main Log instance. */
    template <typename... Args>
    static void debug( Args... args );
    template <typename... Args>
    static void info( Args... args );
    template <typename... Args>
    static void warning( Args... args );
    template <typename... Args>
    static void error( Args... args );

private:
    /** Writes a message type to the log; to be used at the start of every log message. */
    void write_type( MessageType type ) const;

    /** Writes a timestamp to the log. */
    void write_time() const;

    /** A struct wrapping an output stream. This mainly exists to use inheritance for different types of targets. */
    struct Target {
        std::ostream * stream;

        explicit Target( std::ostream * stream );
        virtual ~Target() = default;
    };

    /** A target for output files. FileTarget owns its own output stream through its member 'file'. The Target::stream
     *  member will also point to this output stream. */
    struct FileTarget : Target {
        std::unique_ptr<std::ofstream> file;

        explicit FileTarget( std::string const & filename );
        ~FileTarget() override;
    };

    /// The output target.
    std::unique_ptr<Target> m_target;
    /// Whether colours are enabled when writing logs. The default behaviour for this parameter is to set it to true
    /// unless the target is a file.
    bool m_enable_colours;
};


// Template implementations

template <typename... Args>
void Log::log( MessageType const type, Args... args ) {
    write_time();
    write_type( type );
    *m_target->stream << ' ';
    ((*m_target->stream << std::forward<Args>( args )), ...) << '\n';
}

template <typename... Args>
void Log::log( Args... args ) {
    log( Info, args... );
}

template <typename... Args>
void Log::logf( MessageType const type, std::string const & message, Args const &... args ) {
    write_time();
    write_type( type );
    *m_target->stream << ' ' << std::format( std::runtime_format( message ), args... ) << '\n';
}

template <typename... Args>
void Log::logf( std::string const & message, Args const &... args ) {
    logf( Info, message, args... );
}

template <typename... Args>
void Log::debug( Args... args ) {
    // Implementing this function as a macro, might be more efficient
#ifndef NDEBUG  // Ignore debug logs entirely if NDEBUG is defined
    get_main().log( Debug, args... );
#endif
}

template <typename... Args>
void Log::info( Args... args ) {
    get_main().log( Info, args... );
}

template <typename... Args>
void Log::warning( Args... args ) {
    get_main().log( Warning, args... );
}

template <typename... Args>
void Log::error( Args... args ) {
    get_main().log( Error, args... );
}


#endif //DEMO_TD_LOG_HPP
