#ifndef DEMO_TD_INPUT_MANAGER_HPP
#define DEMO_TD_INPUT_MANAGER_HPP

#include <functional>
#include <map>
#include <set>


class GLFWwindow;


/** Manages inputs from the user, i.e. keyboard and mouse inputs. */
class InputManager
{
public:
    /** Constructor and destructor. */
    explicit InputManager( GLFWwindow * glfw_window = nullptr );
    ~InputManager() = default;

    /** Registers the object's callback function with OpenGL. */
    static void initialise( GLFWwindow * glfw_window );

    /** Registers the callback function as an observer of a key action. Whenever that key is pressed/released/... the
     *  callback function will be called. The returned value is the ID under which the callback function is registered,
     *  which is required if the callback function needs to be removed. The ID is unique per observer_keyboard() call,
     *  and thus probably per callback function, but can represent multiple keys. */
    unsigned int observe_keyboard( int key, std::function<void( int, int )> const & callback );
    unsigned int observe_keyboard( std::set<int> const & keys,
                                                 std::function<void( int, int )> const & callback );

    /** Removes the callback function from the observer list of that key. */
    void forget_keyboard( int key, unsigned int callback_id );
    void forget_keyboard( std::set<int> const & keys, unsigned int callback_id );

    /** Registers the callback function as an observer of the mouse. Whenever the mouse is moved the callback function
     *  will be called. The returned value is the ID under which the callback function is registered, this is required
     *  if the callback needs to be unregistered. */
    unsigned int observe_mouse( std::function<void( int, int )> const & callback );

    /** Removes the callback function from the observer list. */
    void forget_mouse( unsigned int callback_id );

    /** The callback function that handles keyboard inputs. */
    static void handle_keyboard( GLFWwindow * glfw_window, int key, int scancode, int action, int mods );

    /** The callback function that handles mouse inputs. */
    static void handle_mouse( GLFWwindow * glfw_window, double x, double y );

private:
    /// A mapping of received inputs to interested objects/callback functions. Each registered callback function is
    /// given an ID, which is used to identify the callback function in the submaps.
    std::map<int, std::map<unsigned int, std::function<void ( int, int )>>> m_keyboard_observers;
    std::map<unsigned int, std::function<void ( double, double )>> m_mouse_observers;

    /// The next ID to be used when a callback function is registered.
    unsigned int m_next_id;
};


#endif //DEMO_TD_INPUT_MANAGER_HPP
