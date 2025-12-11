#include "time.hpp"


Time::Time()
    : m_initialisation_time { Clock::now() }, m_last_start { Clock::now() }, m_elapsed_time { 0. } {
}

Time & Time::get_instance() {
    static Time instance {};
    return instance;
}

double Time::get_time() const {
    return std::chrono::duration<double>( Clock::now() - m_initialisation_time ).count();
}

void Time::loop_start() {
    Timestamp const current_time { Clock::now() };
    m_elapsed_time = std::chrono::duration<double>( current_time - m_last_start ).count();
    m_last_start = current_time;
}

double Time::get_elapsed_time() const {
    return m_elapsed_time;
}
