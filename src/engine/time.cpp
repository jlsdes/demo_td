#include "time.hpp"


Time::Time()
    : m_initialisation_time { Clock::now() }, m_last_start { Clock::now() }, m_elapsed_time { 0. } {
}

Time & Time::get_instance() {
    static Time instance {};
    return instance;
}

double Time::get_time() {
    Time const & instance { get_instance() };
    return std::chrono::duration<double>( Clock::now() - instance.m_initialisation_time ).count();
}

void Time::loop_start() {
    Time & instance { get_instance() };
    Timestamp const current_time { Clock::now() };
    auto const duration { current_time - instance.m_last_start };
    instance.m_elapsed_time = std::chrono::duration<double>( duration ).count();
    instance.m_last_start = current_time;
}

double Time::get_elapsed_time() {
    return get_instance().m_elapsed_time;
}
