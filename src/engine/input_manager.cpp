#include "input_manager.hpp"

#include "window.hpp"

#include <GLFW/glfw3.h>

#include <format>
#include <ranges>
#include <stdexcept>


InputManager::InputManager( GLFWwindow * const glfw_window )
    : m_keyboard_observers {}, m_mouse_observers {}, m_next_id { 1 } {
    if ( glfw_window != nullptr )
        initialise( glfw_window );
}

void InputManager::initialise( GLFWwindow * const glfw_window ) {
    glfwSetKeyCallback( glfw_window, handle_keyboard );
    glfwSetCursorPosCallback( glfw_window, handle_mouse );
}

unsigned int InputManager::observe_keyboard( int const key, std::function<void( int, int )> const & callback ) {
    // operator[] automatically inserts the key if it isn't present yet
    m_keyboard_observers[key].emplace( m_next_id, callback );
    return m_next_id++;
}

unsigned int InputManager::observe_keyboard( std::set<int> const & keys,
                                             std::function<void( int, int )> const & callback ) {
    for ( auto const & key : keys )
        m_keyboard_observers[key].emplace( m_next_id, callback );
    return m_next_id++;
}

void InputManager::forget_keyboard( int const key, unsigned int const callback_id ) {
    if ( !m_keyboard_observers.contains( key ) )
        throw std::invalid_argument( std::format( "No callbacks were registered for key '{}'.", key ) );
    if ( !m_keyboard_observers.at( key ).contains( callback_id ) )
        throw std::invalid_argument( std::format( "Callback {} was not registered for key '{}'.", callback_id, key ) );
    m_keyboard_observers.at( key ).erase( callback_id );
}

void InputManager::forget_keyboard( std::set<int> const & keys, unsigned int const callback_id ) {
    for ( auto const & key : keys )
        forget_keyboard( key, callback_id );
}

unsigned int InputManager::observe_mouse( std::function<void( int, int )> const & callback ) {
    m_mouse_observers.emplace( m_next_id, callback );
    return m_next_id++;
}

void InputManager::forget_mouse( unsigned int const callback_id ) {
    if ( !m_mouse_observers.contains( callback_id ) )
        throw std::out_of_range( std::format( "Callback {} was not registered for the mouse.", callback_id ) );
    m_mouse_observers.erase( callback_id );
}

/// Helper function for the callbacks that returns the InputManager associated with a GLFW window.
inline InputManager & get_input_manager( GLFWwindow * const glfw_window ) {
    return static_cast<Window *>(glfwGetWindowUserPointer( glfw_window ))->get_input_manager();
}

void InputManager::handle_keyboard( GLFWwindow * const glfw_window,
                                    int const key,
                                    int const scancode,
                                    int const action,
                                    int const mods ) {
    InputManager & input_manager { get_input_manager( glfw_window ) };

    if ( input_manager.m_keyboard_observers.contains( key ) ) {
        auto const & callbacks { input_manager.m_keyboard_observers.at( key ) };
        for ( auto const & callback : std::views::values( callbacks ) )
            callback( key, action );
    }
}

void InputManager::handle_mouse( GLFWwindow * const glfw_window,
                                 double const x,
                                 double const y ) {
    InputManager & input_manager { get_input_manager( glfw_window ) };
    for ( auto const & callback : std::views::values( input_manager.m_mouse_observers ) )
        callback( x, y );
}
