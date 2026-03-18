#pragma once

#include <thread>
#include <vector>

#include <glm/glm.hpp>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::u8vec4 colour;
};


class Mesh {
public:
    explicit Mesh( std::vector<Vertex> const & vertices, std::vector<unsigned int> const & indices = {} );
    ~Mesh();

    Mesh( Mesh const & ) = delete;
    Mesh( Mesh && ) = delete;
    Mesh & operator=( Mesh const & ) = delete;
    Mesh & operator=( Mesh && ) = delete;

    void draw();

private:
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;

    unsigned long m_count;

    // OpenGL calls using these objects must all happen on the same thread
    std::thread::id const m_home_thread;
};
