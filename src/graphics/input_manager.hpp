#ifndef DEMO_TD_INPUT_MANAGER_HPP
#define DEMO_TD_INPUT_MANAGER_HPP

#include <array>
#include <functional>
#include <map>
#include <ranges>
#include <variant>

#include <glad/gl.h>
#include <GLFW/glfw3.h>


/** The supported input types. */
enum InputType : unsigned char {
    KeyboardInput,
    MouseButtonInput,
    CursorInput,
    ScrollInput,
    FirstInputType = KeyboardInput, ///< Utility value for iterating over the input types.
    LastInputType = ScrollInput ///< Utility value for iterating over the input types.
};


/// The desired callback function types for each type of input.
using KeyCallbackFunction = std::function<void( int, int )>;
using MouseCallbackFunction = std::function<void( int, int )>;
using CursorCallbackFunction = std::function<void( double, double )>;
using ScrollCallbackFunction = std::function<void( double, double )>;


/// A (type-safe) union of the various callback functions.
using CallbackFunction = std::variant<KeyCallbackFunction, MouseCallbackFunction, CursorCallbackFunction,
                                      ScrollCallbackFunction>;


/// Links the input types with their associated callback functions.
template <InputType t_type>
using InputHandler = std::variant_alternative_t<t_type, CallbackFunction>;


/** Manages inputs from the user, i.e. keyboard and mouse inputs. */
class InputManager {
public:
    /** Constructor and destructor. */
    explicit InputManager();
    ~InputManager() = default;

    /** Registers the object's callback function with OpenGL. */
    static void initialise( GLFWwindow * glfw_window );

    /** Registers a callback function for an input event. The 'type' argument and the underlying type of 'callback' are
     *  assumed to, e.g. if 'type' is ScrollInput then 'callback.scroll_function' is used. The 'key' parameter is only
     *  used for input types that require it (KeyboardInput and MouseButtonInput), otherwise it is ignored.
     *  The returned value is the callback ID, which is required to remove the callback function again. */
    template <InputType t_type>
    [[nodiscard]] unsigned int observe_input( InputHandler<t_type> const & callback, int key = 0 );

    /** Removes the registered callback function associated with the ID. */
    void forget_input( unsigned int callback_id );

    /** Returns the input type and key under which the callback is registered. */
    [[nodiscard]] std::pair<InputType, int> get_input_data( unsigned int callback_id ) const;

private:
    /** Callback functions for the various input types. */
    static void handle_keyboard( GLFWwindow * window, int key, int, int action, int );
    static void handle_mouse_button( GLFWwindow * window, int button, int action, int );
    static void handle_cursor( GLFWwindow * window, double x, double y );
    static void handle_scroll( GLFWwindow * window, double x_offset, double y_offset );

    /// Utility constants for easier access to 'm_observers'.
    static unsigned int constexpr max_number_keys { GLFW_KEY_LAST + 1 };
    static unsigned int constexpr max_number_buttons { GLFW_MOUSE_BUTTON_LAST + 1 };
    static unsigned int constexpr max_number_inputs { max_number_keys + max_number_buttons + 1 + 1 };
    static unsigned int constexpr type_offsets[] { 0, max_number_keys, max_number_inputs - 2, max_number_inputs - 1 };
    static bool constexpr type_has_key[] { true, true, false, false };

    /** Returns the index to 'm_observers' for the given inputs. */
    [[nodiscard]] static unsigned int constexpr compute_index( InputType type, int key = 0 );

    /** Converts a callback function into the CallbackFunction type. The provided callback function must of course have the
     *  correct signature. */
    template <InputType t_type>
    static CallbackFunction make_callback( InputHandler<t_type> const & callback );

    /** Notifies all observers registered for 't_type' and 'key'. If the callback function requires has the key/
     *  button as its first parameter, then it must be passed in twice: first as 'key', and second as the first value of
     *  'values'. */
    template <InputType t_type, typename... ValueTypes>
        requires std::is_invocable_v<InputHandler<t_type>, ValueTypes...>
    void notify_observers( int key, ValueTypes &&... values );

    /// The callback functions for each of the input type in one large array. Each of the input types have a single
    /// contiguous block of observers, which start at the offsets defined above. Multiple observers can observe the same
    /// input, and because of that each specific input has a map of callback functions.
    std::array<std::map<unsigned int, CallbackFunction>, max_number_inputs> m_observers;

    /// A mapping of callback IDs to array indices.
    std::map<unsigned int, unsigned int> m_bindings;

    /// The next ID to be used when a callback function is registered. The first ID generated is 1, and increases
    /// consecutively. As such, 0 should never be a valid ID and can be used to indicate the absence of an ID.
    unsigned int m_next_id;
};


// Template definition(s)

template <InputType t_type>
CallbackFunction InputManager::make_callback( InputHandler<t_type> const & callback ) {
    return CallbackFunction { std::in_place_index<t_type>, callback };
}

template <InputType t_type>
unsigned int InputManager::observe_input( InputHandler<t_type> const & callback, int const key ) {
    unsigned int const index { compute_index( t_type, key ) };
    m_observers.at( index ).emplace( m_next_id, CallbackFunction { std::in_place_index<t_type>, callback } );
    m_bindings.emplace( m_next_id, index );
    return m_next_id++;
}

template <InputType t_type, typename... ValueTypes> requires std::is_invocable_v<InputHandler<t_type>, ValueTypes...>
void InputManager::notify_observers( int const key, ValueTypes &&... values ) {
    auto & observers { m_observers.at( compute_index( t_type, key ) ) };
    for ( auto & callback : std::views::values( observers ) )
        std::get<t_type>( callback )( values... );
}

unsigned int constexpr InputManager::compute_index( InputType const type, int const key ) {
    return type_offsets[type] + (type_has_key[type] ? key : 0u);
}


#endif //DEMO_TD_INPUT_MANAGER_HPP
