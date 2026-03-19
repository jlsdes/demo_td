#include "input_manager.hpp"

#include "utils/error.hpp"

#include <cassert>
#include <format>
#include <functional>
#include <print>
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
    glfwSetWindowUserPointer( glfw_window, this );

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

    switch ( observer.callback.index() ) {
    case Observer::Keyboard:
        assert( observer.key >= 0 and observer.key <= GLFW_KEY_LAST );
        m_keyboard_observers[observer.key].emplace( &std::get<Observer::Keyboard>( observer.callback ) );
        break;

    case Observer::MouseButton:
        assert( observer.key >= 0 and observer.key <= GLFW_MOUSE_BUTTON_LAST );
        m_mouse_button_observers[observer.key].emplace( &std::get<Observer::MouseButton>( observer.callback ) );
        break;

    case Observer::CursorPosition:
        m_cursor_position_observers.emplace( &std::get<Observer::CursorPosition>( observer.callback ) );
        break;

    case Observer::Scroll:
        m_scroll_observers.emplace( &std::get<Observer::Scroll>( observer.callback ) );
        break;

    case Observer::Resize:
        m_resize_observers.emplace( &std::get<Observer::Resize>( observer.callback ) );
        break;

    case Observer::Close:
        m_close_observers.emplace( &std::get<Observer::Close>( observer.callback ) );
        break;

    case Observer::None:
        break;

    default:
        throw std::invalid_argument( "Unknown observer type encountered." );
    }

    if ( id == m_observers.size() )
        m_observers.emplace_back( observer );
    else
        m_observers[id] = observer;

    return id;
}

void InputManager::remove_observer( unsigned int observer_id ) {
    if ( observer_id >= m_observers.size() )
        throw std::invalid_argument( std::format( "Observer {} has never existed.", observer_id ) );
    if ( m_observers[observer_id].callback.index() == Observer::None )
        throw std::invalid_argument( std::format( "Observer {} does not exist.", observer_id ) );

    auto & observer { m_observers[observer_id] };

    switch ( observer.callback.index() ) {
    case Observer::Keyboard:
        m_keyboard_observers[observer.key].erase( &std::get<Observer::Keyboard>( observer.callback ) );
        break;

    case Observer::MouseButton:
        m_mouse_button_observers[observer.key].erase( &std::get<Observer::MouseButton>( observer.callback ) );
        break;

    case Observer::CursorPosition:
        m_cursor_position_observers.erase( &std::get<Observer::CursorPosition>( observer.callback ) );
        break;

    case Observer::Scroll:
        m_scroll_observers.erase( &std::get<Observer::Scroll>( observer.callback ) );
        break;

    case Observer::Resize:
        m_resize_observers.erase( &std::get<Observer::Resize>( observer.callback ) );
        break;

    case Observer::Close:
        m_close_observers.erase( &std::get<Observer::Close>( observer.callback ) );
        break;

    case Observer::None:
        break;

    default:
        throw std::invalid_argument( "Unknown observer type encountered." );
    }

    m_observers[observer_id] = { nullptr, -1 };
}

static InputManager & get_manager( GLFWwindow * const window ) {
    return *reinterpret_cast<InputManager *>( glfwGetWindowUserPointer( window ) );
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
    for ( auto const & callback : get_manager( window ).m_keyboard_observers[key] ) {
        bool const success { try_call( *callback, key, scancode, action, mods ) };

        if ( not success ) {
            std::string const key_name { glfwGetKeyName( key, scancode ) };
            std::string const mod_names { get_mod_names( mods ) };
            print_error( "Failed to call keyboard observer: {} {}{}", actions[action], key_name, mod_names );
        }
    }
}

void InputManager::press_mouse( GLFWwindow * const window, int const button, int const action, int const mods ) {
    for ( auto const & callback : get_manager( window ).m_mouse_button_observers[button] ) {
        bool const success { try_call( *callback, button, action, mods ) };

        if ( not success ) {
            std::string const mod_names { get_mod_names( mods ) };
            print_error( "Failed to call mouse button observer: {} {}{}", actions[action], button, mod_names );
        }
    }
}

void InputManager::move_cursor( GLFWwindow * const window, double const x_position, double const y_position ) {
    for ( auto const & callback : get_manager( window ).m_cursor_position_observers )
        try_call( *callback, x_position, y_position );
}

void InputManager::scroll( GLFWwindow * const window, double const x, double const y ) {
    for ( auto const & callback : get_manager( window ).m_cursor_position_observers )
        try_call( *callback, x, y );
}

void InputManager::resize( GLFWwindow * const window, int const width, int const height ) {
    for ( auto const & callback : get_manager( window ).m_resize_observers )
        try_call( *callback, width, height );
}

void InputManager::close( GLFWwindow * const window ) {
    for ( auto const & callback : get_manager( window ).m_close_observers )
        try_call( *callback );
}
