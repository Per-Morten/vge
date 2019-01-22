#include <vge_shader.h>
#include <string>
#include <sstream>
#include <fstream>

/// TODO: Make use of C-style file handling, and own string class.

std::string
load_source(const char* shader_path)
{
    std::ifstream file(shader_path);
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint
compile_shader(const char* src, GLenum type)
{
    auto shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        VGE_ERROR("%s", info_log);
    }
    return shader;
}

vge::shader::shader(const char* vertex_path,
                    const char* fragment_path) VGE_NOEXCEPT
{
    const auto vertex_src = load_source(vertex_path);
    const auto fragment_src = load_source(fragment_path);
    auto vertex_shader = compile_shader(vertex_src.c_str(), GL_VERTEX_SHADER);
    auto fragment_shader = compile_shader(fragment_src.c_str(), GL_FRAGMENT_SHADER);

    handle = glCreateProgram();
    glAttachShader(handle, vertex_shader);
    glAttachShader(handle, fragment_shader);
    glLinkProgram(handle);

    int success;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar info_log[512];
        glGetProgramInfoLog(handle, 512, NULL, info_log);
        VGE_ERROR("%s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

vge::shader::shader(shader&& other) VGE_NOEXCEPT
{
    handle = other.handle;
    other.handle = 0;
}

vge::shader&
vge::shader::operator=(shader&& other) VGE_NOEXCEPT
{
    if (this != &other)
    {
        glDeleteProgram(handle);
        handle = other.handle;
        other.handle = 0;
    }
    return *this;
}

vge::shader::~shader() VGE_NOEXCEPT
{
    glDeleteProgram(handle);
}

void
vge::shader::bind()
{
    glUseProgram(handle);
}
