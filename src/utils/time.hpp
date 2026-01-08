#ifndef DEMO_TD_TIME_HPP
#define DEMO_TD_TIME_HPP

#include <chrono>


/// Type aliases because the chrono library is quite verbose.
using Clock = std::chrono::steady_clock;
using Timestamp = std::chrono::time_point<Clock>;


/** A singleton that provides time-keeping features for other objects. */
class Time {
    /// Private constructor and destructor; use Time::get_instance() instead.
    Time();
    ~Time() = default;

public:
    /// Deleted copy and move constructors and assignment operators.
    Time( Time const & ) = delete;
    Time & operator=( Time const & ) = delete;
    Time( Time && ) = delete;
    Time & operator=( Time && ) = delete;

    /** Returns a reference to the same Time object every time. */
    inline static Time & get_instance();

    /** Returns the current time in seconds since the initialisation of the Time class. */
    [[nodiscard]] static double get_time();

    /** Keeps track of when the main program loop starts, and how much time has elapsed. This function is to be called
     *  at the start of the main loop, such that get_elapsed_time() is correct. */
    static void loop_start();

    /** Returns the time in seconds that has elapsed since the last main loop. */
    [[nodiscard]] static double get_elapsed_time();

private:
    /// The timestamp of the approximate start of the program.
    Timestamp const m_initialisation_time;
    /// The start of the last main loop, i.e. the timestamp of the last call to loop_start().
    Timestamp m_last_start;
    /// The time that has elapsed between the starts of the last two loops.
    double m_elapsed_time;
};


#endif //DEMO_TD_TIME_HPP
