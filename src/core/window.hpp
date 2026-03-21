#pragma once


class GLFWwindow;


class Window {
public:
    Window();
    ~Window();

    void focus() const;
    void draw() const;

    [[nodiscard]] bool is_closing() const;

private:
    GLFWwindow * const m_window;

    /// Initialises the window after the entire window context has been created.
    /// Called by WindowContext.
    void initialise();

    friend struct WindowContext;
};
