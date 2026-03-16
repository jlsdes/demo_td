#pragma once

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on


class Window {
public:
    Window();
    ~Window();

    void focus() const;
    void draw() const;

    [[nodiscard]] bool is_closing() const;

private:
    GLFWwindow * const m_window;
};
