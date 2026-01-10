#include "time.hpp"


double duration_to_double( std::chrono::duration<double> const & duration ) {
    return duration.count();
}

Time::Time()
    : m_initialisation_time { Clock::now() }, m_last_start { m_initialisation_time }, m_elapsed_time { 0. } {}

Time & Time::get_instance() {
    static Time instance {};
    return instance;
}

double Time::get_time() {
    Time const & instance { get_instance() };
    return duration_to_double( Clock::now() - instance.m_initialisation_time );
}

double Time::loop_start() {
    Time & instance { get_instance() };
    Timestamp const current_time { Clock::now() };
    // The elapsed time for the very first iteration should simply remain 0, rather than some arbitrary time period
    // spent initialising the program
    if ( instance.m_last_start == instance.m_initialisation_time ) {
        instance.m_elapsed_time = 0.;
    } else {
        auto const duration { current_time - instance.m_last_start };
        instance.m_elapsed_time = std::chrono::duration<double>( duration ).count();
    }
    instance.m_last_start = current_time;
    return duration_to_double( current_time - instance.m_initialisation_time );
}

double Time::get_elapsed_time() {
    return get_instance().m_elapsed_time;
}
