#ifndef DEMO_TD_SHADER_HPP
#define DEMO_TD_SHADER_HPP


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
