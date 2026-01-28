#ifndef DEMO_TD_WINDOW_HPP
#define DEMO_TD_WINDOW_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "input_manager.hpp"


/** A simple window class. */
class Window {
public:
    /** Constructors; if no parameters are provided, then the value will be taken from Config. If this fails, an
     *  exception will be thrown. */
    Window();
    Window( unsigned int width, unsigned int height, char const * title );

    ~Window();

    /** Makes the window the active one. */
    void focus() const;

    /** Clears/Renders a new frame; to be called before/after drawing everything respectively. */
    void clear() const;
    void render() const;

    /** Changes the size of the window. */
    void resize( unsigned int width, unsigned int height ) const;

    /** Sets the window to be closed as soon as possible. */
    void close() const;

    /** Returns whether the window is closing. */
    [[nodiscard]] bool is_closing() const;

    /** Returns the window's input manager. */
    InputManager & get_input_manager();

    /** Resizing callback function. */
    static void resize_callback( GLFWwindow * glfw_window, int width, int height );

private:
    /// The underlying GLFW window object.
    GLFWwindow * m_window;

    /// The attached input manager for this window.
    InputManager m_input_manager;
};


#endif //DEMO_TD_WINDOW_HPP
