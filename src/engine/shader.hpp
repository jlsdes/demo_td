#ifndef DEMO_TD_SHADER_HPP
#define DEMO_TD_SHADER_HPP

#include <glm/glm.hpp>

#include <filesystem>


/** A basic shader handler that builds and runs shaders. */
class Shader
{
protected:
    /** Constructor. */
    Shader();

    /** Destructor. */
    ~Shader();

public:
    /** Activates the shader. */
    void use() const;

    /** Sets a uniform value, implemented for a selection of types. */
    void set_uniform( char const * name, bool value ) const;
    void set_uniform( char const * name, int value ) const;
    void set_uniform( char const * name, unsigned int value ) const;
    void set_uniform( char const * name, float value ) const;
    void set_uniform( char const * name, double value ) const;

    void set_uniform( char const * name, glm::mat4 const & value ) const;

    /** Returns a path to the directory containing this project's shader files. */
    static std::filesystem::path get_shader_directory();

private:
    /** Returns the location of the uniform. */
    int get_uniform_location( char const * name ) const;

protected:
    /// The underlying GL shader program.
    unsigned int const m_program;
};


/** A basic shader handler that builds and runs graphics shaders specifically. */
class GraphicsShader : public Shader
{
public:
    /** Constructor. */
    GraphicsShader( char const * vertex_path, char const * fragment_path );

    /** Destructor. */
    ~GraphicsShader() = default;
};


/** Compiles a single shader script. */
unsigned int compile_shader( unsigned int type, char const * file_name );


#endif //DEMO_TD_SHADER_HPP
