#ifndef DEMO_TD_WINDOW_HPP
#define DEMO_TD_WINDOW_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>


/** A simple window class. */
class Window
{
public:
    /** The main constructor. */
    Window( unsigned int width, unsigned int height, char const * title );

    /** Destructor. */
    ~Window();

    /** Makes the window the active one. */
    void focus() const;

    /** Renders a new frame after all drawing calls (should) have been made. */
    void render() const;

    /** Changes the size of the window. */
    void resize( unsigned int width, unsigned int height ) const;

    /** Sets the window to be closed as soon as possible. */
    void close() const;

    /** Returns whether the window is closing. */
    [[nodiscard]] bool is_closing() const;

    /** Resizing callback function. */
    static void resize_callback( GLFWwindow * glfw_window, int width, int height );

    /** Keyboard actions callback function. */
    static void keyboard_callback( GLFWwindow * glfw_window, int key, int scancode, int action, int mods );

private:
    /// The underlying GLFW window object.
    GLFWwindow * m_window;
};


#endif //DEMO_TD_WINDOW_HPP
