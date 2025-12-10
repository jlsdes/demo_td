#include "input_manager.hpp"

#include <GLFW/glfw3.h>

#include <format>
#include <ranges>
#include <stdexcept>


InputManager::InputManager()
    : m_keyboard_observers {}, m_mouse_observers {}, m_next_id { 1 }
{
}

InputManager & InputManager::get_instance()
{
    static InputManager instance;
    return instance;
}

void InputManager::initialise( GLFWwindow * glfw_window )
{
    glfwSetKeyCallback( glfw_window, handle_keyboard );
    glfwSetCursorPosCallback( glfw_window, handle_mouse );
}

unsigned int InputManager::observe_keyboard( int const key, std::function<void( int, int )> const & callback )
{
    if ( !m_keyboard_observers.contains( key ) )
        m_keyboard_observers.emplace();
    m_keyboard_observers.at( key ).emplace( m_next_id, callback );
    return m_next_id++;
}

unsigned int InputManager::observe_keyboard( std::set<int> const & keys,
                                             std::function<void( int, int )> const & callback )
{
    for ( auto const & key : keys ) {
        if ( !m_keyboard_observers.contains( key ) )
            m_keyboard_observers.emplace();
        m_keyboard_observers.at( key ).emplace( m_next_id, callback );
    }
    return m_next_id++;
}

void InputManager::forget_keyboard( int const key, unsigned int const callback_id )
{
    if ( !m_keyboard_observers.contains( key ) )
        throw std::invalid_argument( std::format( "No callbacks were registered for key '{}'.", key ) );
    if ( !m_keyboard_observers.at( key ).contains( callback_id ) )
        throw std::invalid_argument( std::format( "Callback {} was not registered for key '{}'.", callback_id, key ) );
    m_keyboard_observers.at( key ).erase( callback_id );
}

void InputManager::forget_keyboard( std::set<int> const & keys, unsigned int const callback_id )
{
    for ( auto const & key : keys )
        forget_keyboard( key, callback_id );
}

unsigned int InputManager::observe_mouse( std::function<void( int, int )> const & callback )
{
    m_mouse_observers.emplace( m_next_id, callback );
    return m_next_id++;
}

void InputManager::forget_mouse( unsigned int const callback_id )
{
    if ( !m_mouse_observers.contains( callback_id ) )
        throw std::out_of_range( std::format( "Callback {} was not registered for the mouse.", callback_id ) );
    m_mouse_observers.erase( callback_id );
}

void InputManager::handle_keyboard( GLFWwindow * const window,
                                    int const key,
                                    int const scancode,
                                    int const action,
                                    int const mods )
{
    InputManager & input_manager { get_instance() };

    if ( input_manager.m_keyboard_observers.contains( key ) ) {
        auto const & callbacks { input_manager.m_keyboard_observers.at(key) };
        for ( auto const & callback : callbacks | std::views::values )
            callback( key, action );
    }
}

void InputManager::handle_mouse( GLFWwindow * const window,
                                 double const x,
                                 double const y )
{
}
