#include "input_manager.hpp"
#include "window_context.hpp"

#include "utils/error.hpp"

#include <cassert>
#include <format>
#include <functional>
#include <stdexcept>

#include <GLFW/glfw3.h>


InputManager::InputManager()
        : m_observers {},
          m_keyboard_observers {},
          m_mouse_button_observers {},
          m_cursor_position_observers {},
          m_scroll_observers {},
          m_resize_observers {},
          m_close_observers {} {
    GLFWwindow * const glfw_window { glfwGetCurrentContext() };

    glfwSetKeyCallback( glfw_window, &InputManager::press_key );
    glfwSetMouseButtonCallback( glfw_window, &InputManager::press_mouse );
    glfwSetCursorPosCallback( glfw_window, &InputManager::move_cursor );
    glfwSetScrollCallback( glfw_window, &InputManager::scroll );
    glfwSetWindowSizeCallback( glfw_window, &InputManager::resize );
    glfwSetWindowCloseCallback( glfw_window, &InputManager::close );
}

InputManager::~InputManager() {}

unsigned int InputManager::add_observer( Observer && observer ) {
    unsigned int id { 0 };
    for ( ; id < m_observers.size(); ++id ) {
        if ( m_observers[id].callback.index() == Observer::None )
            break;
    }

    if ( id == m_observers.size() )
        m_observers.emplace_back( observer );
    else
        m_observers[id] = observer;

    switch ( observer.callback.index() ) {
    case Observer::Keyboard:
        assert( observer.key >= 0 and observer.key <= GLFW_KEY_LAST );
        m_keyboard_observers[observer.key].emplace( id );
        break;

    case Observer::MouseButton:
        assert( observer.key >= 0 and observer.key <= GLFW_MOUSE_BUTTON_LAST );
        m_mouse_button_observers[observer.key].emplace( id );
        break;

    case Observer::CursorPosition:
        m_cursor_position_observers.emplace( id );
        break;

    case Observer::Scroll:
        m_scroll_observers.emplace( id );
        break;

    case Observer::Resize:
        m_resize_observers.emplace( id );
        break;

    case Observer::Close:
        m_close_observers.emplace( id );
        break;

    case Observer::None:
        break;

    default:
        throw std::invalid_argument( "Unknown observer type encountered." );
    }

    return id;
}

void InputManager::remove_observer( unsigned int observer_id ) {
    if ( observer_id >= m_observers.size() )
        throw std::invalid_argument( std::format( "Observer {} has never existed.", observer_id ) );
    if ( m_observers[observer_id].callback.index() == Observer::None )
        throw std::invalid_argument( std::format( "Observer {} does not exist.", observer_id ) );

    auto & [callback, key] { m_observers[observer_id] };

    switch ( callback.index() ) {
    case Observer::Keyboard:
        m_keyboard_observers[key].erase( observer_id );
        break;

    case Observer::MouseButton:
        m_mouse_button_observers[key].erase( observer_id );
        break;

    case Observer::CursorPosition:
        m_cursor_position_observers.erase( observer_id );
        break;

    case Observer::Scroll:
        m_scroll_observers.erase( observer_id );
        break;

    case Observer::Resize:
        m_resize_observers.erase( observer_id );
        break;

    case Observer::Close:
        m_close_observers.erase( observer_id );
        break;

    case Observer::None:
        break;

    default:
        throw std::invalid_argument( "Unknown observer type encountered." );
    }

    m_observers[observer_id] = { nullptr, -1 };
}

static InputManager & get_manager( GLFWwindow * const window ) {
    return reinterpret_cast<WindowContext *>( glfwGetWindowUserPointer( window ) )->input_manager;
}

template <typename Callback, typename... Args> static bool try_call( Callback const & callback, Args &&... args ) {
    try {
        callback( args... );
    } catch ( std::bad_function_call const & error ) {
        return false;
    }
    return true;
}

static std::string constexpr get_mod_names( int const mods ) {
    std::string mod_names {};
    if ( mods & 0x01 )
        mod_names += "-<shift>";
    if ( mods & 0x02 )
        mod_names += "-<control>";
    if ( mods & 0x04 )
        mod_names += "-<alt>";
    if ( mods & 0x08 )
        mod_names += "-<super>";
    if ( mods & 0x10 )
        mod_names += "-<caps_lock>";
    if ( mods & 0x20 )
        mod_names += "-<num_lock>";
    return mod_names;
}

std::string constexpr actions[3] { "release", "press", "repeat" };

void InputManager::press_key( GLFWwindow * const window, int const key, int const scancode, int const action,
                              int const mods ) {
    auto const & input_manager { get_manager( window ) };
    for ( unsigned int const id : input_manager.m_keyboard_observers[key] ) {
        auto const & callback { std::get<Observer::Keyboard>( input_manager.m_observers[id].callback ) };

        if ( not try_call( callback, key, scancode, action, mods ) ) {
            std::string const key_name { glfwGetKeyName( key, scancode ) };
            std::string const mod_names { get_mod_names( mods ) };
            print_error( "Failed to call keyboard observer {}: {} {}{}", id, actions[action], key_name, mod_names );
        }
    }
}

void InputManager::press_mouse( GLFWwindow * const window, int const button, int const action, int const mods ) {
    auto const & input_manager { get_manager( window ) };
    for ( unsigned int const id : input_manager.m_mouse_button_observers[button] ) {
        auto const & callback { std::get<Observer::MouseButton>( input_manager.m_observers[id].callback ) };

        if ( not try_call( callback, button, action, mods ) ) {
            std::string const mod_names { get_mod_names( mods ) };
            print_error( "Failed to call mouse button observer {}: {} {}{}", id, actions[action], button, mod_names );
        }
    }
}

void InputManager::move_cursor( GLFWwindow * const window, double const x_position, double const y_position ) {
    auto const & input_manager { get_manager( window ) };
    for ( unsigned int const id : input_manager.m_cursor_position_observers ) {
        auto const & callback { std::get<Observer::CursorPosition>( input_manager.m_observers[id].callback ) };

        if ( not try_call( callback, x_position, y_position ) )
            print_error( "Failed to call cursor position observer {}", id );
    }
}

void InputManager::scroll( GLFWwindow * const window, double const x, double const y ) {
    auto const & input_manager { get_manager( window ) };
    for ( unsigned int const id : input_manager.m_cursor_position_observers ) {
        auto const & callback { std::get<Observer::Scroll>( input_manager.m_observers[id].callback ) };

        if ( not try_call( callback, x, y ) )
            print_error( "Failed to call scroll observer {}", id );
    }
}

void InputManager::resize( GLFWwindow * const window, int const width, int const height ) {
    auto const & input_manager { get_manager( window ) };
    for ( unsigned int const id : input_manager.m_resize_observers ) {
        auto const & callback { std::get<Observer::Resize>( input_manager.m_observers[id].callback ) };

        if ( not try_call( callback, width, height ) )
            print_error( "Failed to call resize observer {}", id );
    }
}

void InputManager::close( GLFWwindow * const window ) {
    auto const & input_manager { get_manager( window ) };
    for ( unsigned int const id : input_manager.m_close_observers ) {
        auto const & callback { std::get<Observer::Close>( input_manager.m_observers[id].callback ) };

        if ( not try_call( callback ) )
            print_error( "Failed to call close observer {}", id );
    }
}
