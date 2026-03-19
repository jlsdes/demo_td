#pragma once

#include <array>
#include <functional>
#include <unordered_set>
#include <variant>
#include <vector>

#include <GLFW/glfw3.h>


using KeyboardObserver       = std::function<void( int, int, int, int )>;
using MouseButtonObserver    = std::function<void( int, int, int )>;
using CursorPositionObserver = std::function<void( double, double )>;
using ScrollObserver         = std::function<void( double, double )>;
using ResizeObserver         = std::function<void( int, int )>;
using CloseObserver          = std::function<void()>;


struct Observer {
    enum Type {
        Keyboard,
        MouseButton,
        CursorPosition,
        Scroll,
        Resize,
        Close,
        None,
    };

    using Callback = std::variant<KeyboardObserver, MouseButtonObserver, CursorPositionObserver, ScrollObserver,
                                  ResizeObserver, CloseObserver, std::nullptr_t>;
    Callback callback { nullptr };

    int key { -1 };
};


class InputManager {
public:
    InputManager();
    ~InputManager();

    InputManager( InputManager const & )             = delete;
    InputManager( InputManager && )                  = delete;
    InputManager & operator=( InputManager const & ) = delete;
    InputManager & operator=( InputManager && )      = delete;

    /// Adds an input observer to the observer list. The returned value is the observer ID, which is needed to remove
    /// the observer again.
    unsigned int add_observer( Observer && observer );

    /// Removes the observer that was registered under the given ID. This ID is returned by add_observer().
    void remove_observer( unsigned int observer_id );

private:
    std::vector<Observer> m_observers;

    std::array<std::unordered_set<KeyboardObserver *>, GLFW_KEY_LAST + 1> m_keyboard_observers;
    std::array<std::unordered_set<MouseButtonObserver *>, GLFW_MOUSE_BUTTON_LAST + 1> m_mouse_button_observers;
    std::unordered_set<CursorPositionObserver *> m_cursor_position_observers;
    std::unordered_set<ScrollObserver *> m_scroll_observers;
    std::unordered_set<ResizeObserver *> m_resize_observers;
    std::unordered_set<CloseObserver *> m_close_observers;
};
