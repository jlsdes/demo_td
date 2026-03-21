#pragma once

#include <array>
#include <functional>
#include <unordered_set>
#include <variant>
#include <vector>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on


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

    /// Callback functions to be called by GLFW. Can also be called like normal functions, for whatever reason.
    static void press_key( GLFWwindow * window, int key, int scancode, int action, int mods );
    static void press_mouse( GLFWwindow * window, int button, int action, int mods );
    static void move_cursor( GLFWwindow * window, double x_position, double y_position );
    static void scroll( GLFWwindow * window, double x_offset, double y_offset );
    static void resize( GLFWwindow * window, int width, int height );
    static void close( GLFWwindow * window );

private:
    std::vector<Observer> m_observers;

    std::array<std::unordered_set<unsigned int>, GLFW_KEY_LAST + 1> m_keyboard_observers;
    std::array<std::unordered_set<unsigned int>, GLFW_MOUSE_BUTTON_LAST + 1> m_mouse_button_observers;
    std::unordered_set<unsigned int> m_cursor_position_observers;
    std::unordered_set<unsigned int> m_scroll_observers;
    std::unordered_set<unsigned int> m_resize_observers;
    std::unordered_set<unsigned int> m_close_observers;

    template <Observer::Type ObserverType, typename... Args>
    void notify( std::unordered_set<unsigned int> const & observers, char const * info, Args &&... args ) const;
};
