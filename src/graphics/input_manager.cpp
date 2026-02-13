#include "input_manager.hpp"
#include "window.hpp"
#include "utils/log.hpp"

#include <format>


InputManager::InputManager() : m_observers {}, m_bindings {}, m_next_id { 1 } {}

void InputManager::initialise( GLFWwindow * const glfw_window ) {
    glfwSetKeyCallback( glfw_window, handle_keyboard );
    glfwSetCursorPosCallback( glfw_window, handle_cursor );
}

unsigned int InputManager::observe_input( InputType const type, CallbackFunction const & callback, int const key ) {
    unsigned int const index { compute_index( type, key ) };
    m_bindings.emplace( m_next_id, index );
    m_observers.at( index ).emplace( m_next_id, callback );
    return m_next_id++;
}

void InputManager::forget_input( unsigned int const callback_id ) {
    if ( not m_bindings.contains( callback_id ) ) {
        Log::error( "Attempted to remove an input observer with an ID (", callback_id, ") that isn't registered;"
                    "ignoring." );
        return;
    }
    unsigned int const index { m_bindings.at( callback_id ) };
    m_observers.at( index ).erase( callback_id );
    m_bindings.erase( callback_id );
}

InputManager & get_input_manager( GLFWwindow * const glfw_window ) {
    return static_cast<Window *>(glfwGetWindowUserPointer( glfw_window ))->get_input_manager();
}

void InputManager::handle_keyboard( GLFWwindow * const window, int const key, int, int const action, int ) {
    InputManager & input_manager { get_input_manager( window ) };
    input_manager.notify_observers<KeyboardInput>( key, key, action );
}

void InputManager::handle_cursor( GLFWwindow * const window, double const x, double const y ) {
    InputManager & input_manager { get_input_manager( window ) };
    input_manager.notify_observers<CursorInput>( 0, x, y );
}

unsigned int InputManager::compute_index( InputType const type, int const key ) const {
    return type_offsets[type] + (type_has_key[type] ? key : 0u);
}
