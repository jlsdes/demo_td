#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <map>


struct ShaderSource {
    std::map<std::string, std::string> definitions;
    std::vector<std::string> source;
};


class Shader {
public:
    Shader( std::filesystem::path const & vertex_path, std::filesystem::path const & fragment_path );
    ~Shader() = default;

private:
    std::vector<ShaderSource> m_sources;

    unsigned int m_program;
};
