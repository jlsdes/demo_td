#include "input_manager.hpp"

#include <cassert>
#include <format>
#include <stdexcept>

#include <GLFW/glfw3.h>


InputManager::InputManager()
        : m_observers {},
          m_keyboard_observers {},
          m_mouse_button_observers {},
          m_cursor_position_observers {},
          m_scroll_observers {},
          m_resize_observers {},
          m_close_observers {} {}

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
        m_observers.push_back( std::move( observer ) );
    else
        m_observers[id] = std::move( observer );

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
