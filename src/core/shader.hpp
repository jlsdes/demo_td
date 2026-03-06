#ifndef DEMO_TD_SHADER_HPP
#define DEMO_TD_SHADER_HPP

#include <glm/glm.hpp>

#include <filesystem>
#include <map>
#include <string>


/** A basic shader handler that builds and runs shaders. */
class Shader {
    /** Private constructor; use the static builder function(s) instead. */
    Shader();

public:
    ~Shader();

    Shader( Shader const & ) = delete;
    Shader & operator=( Shader const & ) = delete;

    Shader( Shader && other ) noexcept;
    Shader & operator=( Shader && other ) noexcept;

    /** Activates the shader. */
    void use() const;

    /** Sets a uniform value, implemented for a selection of types. */
    void set_uniform( char const * name, bool value ) const;
    void set_uniform( char const * name, int value ) const;
    void set_uniform( char const * name, unsigned int value ) const;
    void set_uniform( char const * name, float value ) const;
    void set_uniform( char const * name, double value ) const;

    void set_uniform( char const * name, glm::vec2 const & value ) const;
    void set_uniform( char const * name, glm::vec3 const & value ) const;
    void set_uniform( char const * name, glm::vec4 const & value ) const;
    void set_uniform( char const * name, glm::mat2 const & value ) const;
    void set_uniform( char const * name, glm::mat3 const & value ) const;
    void set_uniform( char const * name, glm::mat4 const & value ) const;

    /** Builds a new graphics shader from the given source files. */
    static Shader build_graphics_shader( std::filesystem::path const & vertex_path,
                                         std::filesystem::path const & fragment_path );

private:
    /** Returns the location of the uniform. */
    int get_uniform_location( char const * name ) const;

    /// The underlying GL shader program.
    unsigned int m_program;
};


/** Stores shader programs and provides access to them through IDs. */
class ShaderStore {
public:
    ShaderStore() = default;

    Shader & insert_shader( std::string const & name, Shader && shader );
    Shader & emplace_shader( std::string const & name,
                             std::filesystem::path const & vertex_path,
                             std::filesystem::path const & fragment_path );

    /** Returns a reference to the shader. This can also be used to update the shader. */
    Shader & get_shader( std::string const & name );

    [[nodiscard]] std::map<std::string, Shader>::const_iterator begin() const;
    [[nodiscard]] std::map<std::string, Shader>::const_iterator end() const;

private:
    /// The shaders; the shader IDs correspond to their index in this vector. This is possible because stored shaders
    /// can't be removed.
    std::map<std::string, Shader> m_shaders;
};


#endif //DEMO_TD_SHADER_HPP
