#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>


struct ShaderSource;


class Shader {
public:
    Shader();
    Shader( std::filesystem::path const & vertex_path, std::filesystem::path const & fragment_path );
    ~Shader();

    void add( std::filesystem::path const & path, unsigned int shader_type );
    void build();

    [[nodiscard]] bool has_definition( std::string const & name ) const;
    [[nodiscard]] std::string get_definition( std::string const & name ) const;
    void set_definition( std::string const & name, std::string const & value );

private:
    std::vector<std::unique_ptr<ShaderSource>> m_sources;
    unsigned int m_program;
};
