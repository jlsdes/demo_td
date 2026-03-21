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

std::string constexpr types[] { "keyboard", "mouse button", "cursor", "scroll", "resize", "close" };
std::string constexpr actions[] { "release", "press", "repeat" };

template <Observer::Type ObserverType, typename... Args>
void InputManager::notify( std::unordered_set<unsigned int> const & observers, char const * const info,
                           Args &&... args ) const {
    for ( unsigned int const id : observers ) {
        try {
            std::get<ObserverType>( m_observers[id].callback )( args... );
        } catch ( std::bad_function_call const & error ) {
            print_error( "Failed to call {} observer {} (info: {})", types[ObserverType], id, info );
        }
    }
}

void InputManager::press_key( GLFWwindow * const window, int const key, int const scancode, int const action,
                              int const mods ) {
    auto const key_name { glfwGetKeyName( key, scancode ) };
    std::string const print_name { key_name ? key_name : "<" + std::to_string( key ) + ">" };
    std::string const info { std::format( "{} {}{}", actions[action], print_name, get_mod_names( mods ) ) };

    auto const & manager { get_manager( window ) };
    auto const & observers { manager.m_keyboard_observers[key] };
    manager.notify<Observer::Keyboard>( observers, info.c_str(), key, scancode, action, mods );
}

void InputManager::press_mouse( GLFWwindow * const window, int const button, int const action, int const mods ) {
    std::string const info { std::format( "{} {}{}", actions[action], button, get_mod_names( mods ) ) };

    auto const & manager { get_manager( window ) };
    auto const & observers { manager.m_mouse_button_observers[button] };
    manager.notify<Observer::MouseButton>( observers, info.c_str(), button, action, mods );
}

void InputManager::move_cursor( GLFWwindow * const window, double const x_position, double const y_position ) {
    auto const & manager { get_manager( window ) };
    manager.notify<Observer::CursorPosition>( manager.m_cursor_position_observers, "", x_position, y_position );
}

void InputManager::scroll( GLFWwindow * const window, double const x, double const y ) {
    auto const & manager { get_manager( window ) };
    manager.notify<Observer::Scroll>( manager.m_scroll_observers, "", x, y );
}

void InputManager::resize( GLFWwindow * const window, int const width, int const height ) {
    auto const & manager { get_manager( window ) };
    manager.notify<Observer::Resize>( manager.m_resize_observers, "", width, height );
}

void InputManager::close( GLFWwindow * const window ) {
    auto const & manager { get_manager( window ) };
    manager.notify<Observer::Close>( manager.m_close_observers, "" );
}
