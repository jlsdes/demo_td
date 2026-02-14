#include "input_manager.hpp"
#include "window.hpp"
#include "utils/log.hpp"

#include <format>


InputManager::InputManager() : m_observers {}, m_bindings {}, m_next_id { 1 } {}

void InputManager::initialise( GLFWwindow * const glfw_window ) {
    glfwSetKeyCallback( glfw_window, handle_keyboard );
    glfwSetMouseButtonCallback( glfw_window, handle_mouse_button );
    glfwSetCursorPosCallback( glfw_window, handle_cursor );
    glfwSetScrollCallback( glfw_window, handle_scroll );
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

std::pair<InputType, int> InputManager::get_input_data( unsigned int const callback_id ) const {
    unsigned int const index { m_bindings.at( callback_id ) };
    if ( index == type_offsets[ScrollInput] )
        return { ScrollInput, 0 };
    if ( index == type_offsets[CursorInput] )
        return { CursorInput, 0 };
    if ( index >= type_offsets[MouseButtonInput] )
        return { MouseButtonInput, index - type_offsets[MouseButtonInput] };
    return { KeyboardInput, index };
}

InputManager & get_input_manager( GLFWwindow * const glfw_window ) {
    return static_cast<Window *>(glfwGetWindowUserPointer( glfw_window ))->get_input_manager();
}

void InputManager::handle_keyboard( GLFWwindow * const window,
                                    int const key,
                                    int const scancode,
                                    int const action,
                                    int const mods ) {
    InputManager & input_manager { get_input_manager( window ) };
    input_manager.notify_observers<KeyboardInput>( key, key, action );
}

void InputManager::handle_mouse_button( GLFWwindow * const window,
                                        int const button,
                                        int const action,
                                        int const mods ) {
    InputManager & input_manager { get_input_manager( window ) };
    input_manager.notify_observers<MouseButtonInput>( button, button, action );
}

void InputManager::handle_cursor( GLFWwindow * const window, double const x, double const y ) {
    InputManager & input_manager { get_input_manager( window ) };
    input_manager.notify_observers<CursorInput>( 0, x, y );
}

void InputManager::handle_scroll( GLFWwindow * const window, double const x_offset, double const y_offset ) {
    InputManager & input_manager { get_input_manager( window ) };
    input_manager.notify_observers<ScrollInput>( 0, x_offset, y_offset );
}
