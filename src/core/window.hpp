#pragma once

#include <utility>


class GLFWwindow;
struct WindowContext;


class Window {
public:
    Window();
    ~Window();

    /// Get/set the window's size, defined as (width, height).
    [[nodiscard]] std::pair<unsigned int, unsigned int> get_size() const;
    void set_size( std::pair<unsigned int, unsigned int> const & size );


    void focus() const;
    void draw() const;

    [[nodiscard]] bool is_closing() const;

private:
    GLFWwindow * const m_window;

    unsigned int m_width;
    unsigned int m_height;

    /// Initialises the window after the entire window context has been created.
    /// Called by WindowContext.
    void initialise( WindowContext & context );

    friend struct WindowContext;
};
