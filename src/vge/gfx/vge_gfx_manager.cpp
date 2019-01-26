#include <vge_gfx_manager.h>
#include <vge_gfx_gl.h>
#include <vge_debug.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <stb_image.h>

/////////////////////////////////////////////////
/// Mesh Related
/////////////////////////////////////////////////
struct mesh_gl_data
{
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint uv0TBO;
    GLuint uv1TBO;
};

// TODO: Need a more efficient way to store this data, the meta data is only
// needed for imgui, so no need to store it with the rest.
// Similarly, only the VAO is needed when drawing, so shouldn't store that
// together with the VBO, EBO etc either.
struct mesh_info
{
    vge::gfx_manager::mesh_handle handle;
    vge::gfx_manager::mesh_data mesh_data;
    mesh_gl_data gl_data;
};

static std::vector<mesh_info> g_mesh_table;
static vge::gfx_manager::mesh_handle g_new_mesh_handle;

vge::gfx_manager::mesh_handle
vge::gfx_manager::create_mesh()
{
    auto NewPair = mesh_info();
    NewPair.handle = g_new_mesh_handle++;
    g_mesh_table.push_back(NewPair);
    return NewPair.handle;
}

void
vge::gfx_manager::set_mesh(vge::gfx_manager::mesh_handle handle,
                           vge::gfx_manager::mesh_data data)
{
    auto itr = std::find_if(g_mesh_table.begin(), g_mesh_table.end(),
                            [=](const auto& item)
                            { return item.handle == handle; });

    if (itr == g_mesh_table.end())
    {
        VGE_DEBUG("Handle %d not found", handle);
        return;
    }

    if (!itr->mesh_data.name.empty())
    {
        // Destroy OpenGL stuff
    }

    itr->mesh_data = data;
    mesh_gl_data new_data;

    glGenVertexArrays(1, &new_data.VAO);
    glBindVertexArray(new_data.VAO); // Bind VAO to start "recording" binding calls

    glGenBuffers(1, &new_data.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * data.triangle_count, data.triangles, GL_STATIC_DRAW);

    glGenBuffers(1, &new_data.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, new_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.vertex_count, data.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &new_data.uv0TBO);
    glBindBuffer(GL_ARRAY_BUFFER, new_data.uv0TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * data.vertex_count, data.uv0, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO to avoid messing it up by binding other stuff.

    itr->gl_data = new_data;
}

// TODO: This should be assumed to be async.
void
vge::gfx_manager::draw_mesh(vge::gfx_manager::mesh_handle handle)
{
    auto itr = std::find_if(g_mesh_table.begin(), g_mesh_table.end(),
                            [=](const auto& item)
                            { return item.handle == handle; });

    glBindVertexArray(itr->gl_data.VAO);
    glDrawElements(GL_TRIANGLES, itr->mesh_data.triangle_count, GL_UNSIGNED_INT, 0);
}

///////////////////////////////////////////////////////////
/// Texture Related
///////////////////////////////////////////////////////////
struct texture_info
{
    vge::gfx_manager::texture_handle handle;
    vge::gfx_manager::texture_id texture_id;

    // TODO: Move this extended information into different places to make better use of cache etc.
    std::string filepath;

    int width;
    int height;
};

static int g_new_texture_handle;
static std::vector<texture_info> g_texture_table;

namespace local::texture
{
    texture_info*
    get_texture(vge::gfx_manager::texture_handle handle)
    {
        auto itr = std::find_if(g_texture_table.begin(),
                                g_texture_table.end(),
                                [=](const auto& item)
                                { return item.handle == handle; });

        return (itr != g_texture_table.end())
                   ? &(*itr)
                   : nullptr;
    }
}

vge::gfx_manager::texture_handle
vge::gfx_manager::create_texture()
{
    auto& texture = g_texture_table.emplace_back();
    texture.handle = g_new_texture_handle++;
    return texture.handle;
}

void
vge::gfx_manager::load_texture(vge::gfx_manager::texture_handle handle,
                               const char* filepath)
{
    auto texture = local::texture::get_texture(handle);

    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);
    auto data = stbi_load(filepath, &width, &height, &channels, 0);
    if (!data)
        VGE_ERROR("Could not load image: %s, %s", filepath, stbi_failure_reason());

    texture->filepath = filepath;
    texture->width = width;
    texture->height = height;

    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const auto fmt = (channels < 4) ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

vge::gfx_manager::texture_id
vge::gfx_manager::get_texture_id(vge::gfx_manager::texture_handle handle)
{
    return local::texture::get_texture(handle)->texture_id;
}

///////////////////////////////////////////////////////////
/// Shader Related
///////////////////////////////////////////////////////////
struct program
{
    vge::gfx_manager::shader_handle handle;
    vge::gfx_manager::program_id program_id;
};

struct shader_source
{
    GLuint shader_id;

    GLenum type;
    char file[128];
    char source[1024 * 16];
};

static std::vector<program> g_program_table;
static vge::gfx_manager::shader_handle g_new_program_handle;

static std::vector<shader_source> g_shader_source_table;

namespace local::shader
{
    std::string
    load_source(const char* shader_path)
    {
        std::ifstream file(shader_path);
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    void
    load_source(const char* shader_path, char* buffer, int buff_size)
    {
        VGE_ASSERT(shader_path && buffer && buff_size > 0, "Invalid parameters");

        errno = 0;
        FILE* file = std::fopen(shader_path, "r");
        if (!file || errno != 0)
            VGE_ERROR("Could not open shader file: %s, error: %s", shader_path, strerror(errno));

        std::fseek(file, 0L, SEEK_END);
        auto file_len = std::ftell(file) + 1;
        std::rewind(file);

        VGE_ASSERT(file_len <= buff_size, "Shader is too large for buffer: %s", shader_path);

        buffer[file_len - 1] = '\0';

        if (auto res = (int)std::fread(buffer, sizeof(char), file_len, file); res < file_len - 1)
            VGE_ERROR("Could not read entire shader, res: %d vs %d", res, file_len);

        std::fclose(file);
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

    program*
    get_shader(vge::gfx_manager::shader_handle handle)
    {
        auto itr = std::find_if(g_program_table.begin(), g_program_table.end(),
                                [=](const auto& item)
                                { return item.handle == handle; });

        return (itr != g_program_table.end())
                    ? &(*itr)
                    : nullptr;
    }

    shader_source*
    get_shader_source(GLuint shader_id)
    {
        auto itr = std::find_if(g_shader_source_table.begin(), g_shader_source_table.end(),
                                [=](const auto& item)
                                { return item.shader_id == shader_id; });

        return (itr != g_shader_source_table.end())
                    ? &(*itr)
                    : nullptr;
    }
} // namespace local::shader

vge::gfx_manager::shader_handle
vge::gfx_manager::create_shader()
{
    auto new_data = program();
    new_data.handle = g_new_program_handle++;
    new_data.program_id = glCreateProgram();
    g_program_table.push_back(new_data);

    return new_data.handle;
}

void
vge::gfx_manager::attach_shader(vge::gfx_manager::shader_handle handle,
                                const char* filepath,
                                GLenum type)
{
    VGE_ASSERT(filepath, "filepath cannot be null");

    shader_source tmp;
    tmp.type = type;
    std::strcpy(tmp.file, filepath);
    local::shader::load_source(filepath, tmp.source, sizeof(tmp.source));
    tmp.shader_id = local::shader::compile_shader(tmp.source, type);
    g_shader_source_table.push_back(tmp);

    auto program = local::shader::get_shader(handle);
    VGE_ASSERT(program, "Did not find program with handle: %d", handle);

    glAttachShader(program->program_id, tmp.shader_id);
}

void
vge::gfx_manager::compile_and_link_shader(shader_handle handle)
{
    auto program = local::shader::get_shader(handle);
    VGE_ASSERT(program, "Did not find program with handle: %d", handle);
    glLinkProgram(program->program_id);

    int success;
    glGetProgramiv(program->program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar info_log[512];
        glGetProgramInfoLog(program->program_id, 512, nullptr, info_log);
        VGE_ERROR("%s", info_log);
    }
}

vge::gfx_manager::program_id
vge::gfx_manager::get_shader_id(vge::gfx_manager::shader_handle handle)
{
    auto program = local::shader::get_shader(handle);
    VGE_ASSERT(program, "Did not find program with handle: %d", handle);
    return program->program_id;
}

///////////////////////////////////////////////////////////
/// Introspection related
///////////////////////////////////////////////////////////
// TODO: Based on the introspection stuff I found when developing gfx.h
//       I should remove unneeded meta information in this file.
///      Can at least do this for uniforms!
namespace local::introspection
{
    #define VARIABLE_RENDER_GENERATOR(cputype, count, gltype, glread, glwrite, imguifunc)  \
    {                                                                                      \
        ImGui::Text(#gltype" %s", name);                                                   \
        ImGui::SameLine();                                                                 \
        cputype value[count];                                                              \
        cputype* ptr = &value[0];                                                          \
        glread(program, location, ptr);                                                    \
        if (imguifunc("", ptr))                                                            \
            glwrite(program, location, 1, ptr);                                            \
    }

    static void
    render_uniform_variable(GLuint program, GLenum type, const char* name, GLint location)
    {
        switch (type)
        {
            case GL_FLOAT:
                VARIABLE_RENDER_GENERATOR(float, 1, GL_FLOAT, glGetUniformfv, glProgramUniform1fv, ImGui::DragFloat);
                break;

            case GL_FLOAT_VEC2:
                VARIABLE_RENDER_GENERATOR(float, 2, GL_FLOAT_VEC2, glGetUniformfv, glProgramUniform2fv, ImGui::DragFloat2);
                break;

            case GL_FLOAT_VEC3:
                VARIABLE_RENDER_GENERATOR(float, 3, GL_FLOAT_VEC3, glGetUniformfv, glProgramUniform3fv, ImGui::DragFloat3);
                break;

            case GL_FLOAT_VEC4:
                VARIABLE_RENDER_GENERATOR(float, 4, GL_FLOAT_VEC4, glGetUniformfv, glProgramUniform4fv, ImGui::DragFloat4);
                break;

            case GL_SAMPLER_2D:
                VARIABLE_RENDER_GENERATOR(int, 1, GL_SAMPLER_2D, glGetUniformiv, glProgramUniform1iv, ImGui::InputInt);
                break;

            default:
                ImGui::Text("%s has type %s, which isn't supported yet!", name, vge::gfx::gl_enum_to_string(type));
                break;
        }
    }

    #undef VARIABLE_RENDER_GENERATOR
}

void
vge::gfx_manager::draw_imgui_debug()
{
    if (ImGui::BeginTabBar("GraphicsTab"))
    {
        if (ImGui::BeginTabItem("Meshes"))
        {
            for (const auto& mesh : g_mesh_table)
            {
                char buffer[32];
                std::sprintf(buffer, "Handle: %d", mesh.handle);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Text("name: %s", mesh.mesh_data.name.c_str());
                    ImGui::Text("vertex_count: %d", mesh.mesh_data.vertex_count);
                    ImGui::Text("triangle_count: %d", mesh.mesh_data.triangle_count);

                    if (ImGui::TreeNode("GLData"))
                    {
                        ImGui::Text("VAO: %u", mesh.gl_data.VAO);
                        ImGui::Text("VBO: %u", mesh.gl_data.VBO);
                        ImGui::Text("EBO: %u", mesh.gl_data.EBO);
                        ImGui::Text("uv0TBO: %u", mesh.gl_data.uv0TBO);
                        ImGui::Text("uv0TB1: %u", mesh.gl_data.uv1TBO);

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Data"))
                    {
                        // TODO: Fix logic here, so we don't always use 200(?)
                        ImGui::BeginChild("child", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                        ImGui::Columns(4);
                        ImGui::Text("triangles");
                        ImGui::NextColumn();
                        ImGui::Text("vertices x");
                        ImGui::NextColumn();
                        ImGui::Text("vertices y");
                        ImGui::NextColumn();
                        ImGui::Text("vertices z");
                        ImGui::NextColumn();

                        ImGui::Spacing();
                        ImGui::Separator();

                        int max = std::max(mesh.mesh_data.vertex_count,
                                           mesh.mesh_data.triangle_count);

                        for (int i = 0; i < max; i++)
                        {
                            if (i < mesh.mesh_data.triangle_count)
                            {
                                ImGui::Text("%d", mesh.mesh_data.triangles[i]);
                            }
                            ImGui::NextColumn();

                            // TODO: Really want to indexes together with the vertex corresponding with that index
                            if (i < mesh.mesh_data.vertex_count)
                            {
                                for (int j = 0; j < 3; j++)
                                {
                                    ImGui::Text("% f", *(float*)(&mesh.mesh_data.vertices[i].x + j));
                                    ImGui::NextColumn();
                                }
                            }

                            while (ImGui::GetColumnIndex() > 0 && ImGui::GetColumnIndex() < ImGui::GetColumnsCount())
                            {
                                ImGui::NextColumn();
                            }

                            ImGui::Separator();
                        }
                        ImGui::EndChild();
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Shader Programs"))
        {
            for (const auto& program : g_program_table)
            {
                char buffer[32];
                std::sprintf(buffer, "Handle: %d", program.handle);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Indent();
                    if (ImGui::CollapsingHeader("Uniforms", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        GLint uniform_count;
                        glGetProgramiv(program.program_id, GL_ACTIVE_UNIFORMS, &uniform_count);

                        // Read the length of the longest active uniform.
                        GLint max_name_length;
                        glGetProgramiv(program.program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);

                        VGE_ASSERT(max_name_length < 32, "Currently don't support uniform names of more than 32 characters, it was %d", max_name_length);
                        char name[32];

                        for (int i = 0; i < uniform_count; i++)
                        {
                            GLint ignored;
                            GLenum type;
                            glGetActiveUniform(program.program_id, i, max_name_length, nullptr, &ignored, &type, name);

                            const auto location = glGetUniformLocation(program.program_id, name);
                            ImGui::Indent();
                            ImGui::PushID(i);
                            local::introspection::render_uniform_variable(program.program_id, type, name, location);
                            ImGui::PopID();
                            ImGui::Unindent();
                        }
                    }
                    ImGui::Unindent();

                    ImGui::Indent();
                    if (ImGui::CollapsingHeader("Shaders"))
                    {
                        GLint shader_count;
                        glGetProgramiv(program.program_id, GL_ATTACHED_SHADERS, &shader_count);

                        VGE_ASSERT(shader_count <= 2, "We are currently not expecting more than 2 shaders per shader program, count: %d", shader_count);

                        GLuint attached_shaders[2];
                        glGetAttachedShaders(program.program_id, shader_count, nullptr, attached_shaders);

                        for (const auto& shader_id : attached_shaders)
                        {
                            auto shader = local::shader::get_shader_source(shader_id);
                            VGE_ASSERT(shader, "Source for shader: %u is nullptr", shader_id);

                            ImGui::Indent();
                            auto string_type = gfx::gl_enum_to_string(shader->type);
                            ImGui::PushID(string_type);
                            if (ImGui::CollapsingHeader(string_type))
                            {
                                auto y_size = std::min(ImGui::CalcTextSize(shader->source).y, ImGui::GetTextLineHeightWithSpacing() * 16);

                                if (ImGui::Button("Compile"))
                                {
                                    const char* ptr = shader->source;
                                    glShaderSource(shader_id, 1, &ptr, NULL);
                                    glCompileShader(shader_id);

                                    int success = 0;
                                    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
                                    if (success)
                                    {
                                        glLinkProgram(program.program_id);
                                        glGetProgramiv(program.program_id, GL_LINK_STATUS, &success);
                                        if (!success)
                                        {
                                            // Should probably also get the log length here, so I know how much space I need
                                            GLchar info_log[512];
                                            glGetProgramInfoLog(program.program_id, sizeof(info_log), nullptr, info_log);
                                            VGE_ERROR("Linking error while live editing shader %s", info_log);
                                        }
                                    }
                                }

                                ImGui::InputTextMultiline("##shader", shader->source, sizeof(shader->source), ImVec2(-1.0f, y_size));

                                int success = 0;
                                glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
                                ImGui::Text("Compilation status: %s", (success) ? "Compiled" : "Failed");

                                GLchar info_log[512];
                                GLsizei length = 0;
                                glGetShaderInfoLog(shader_id, sizeof(info_log), &length, info_log);
                                if (length)
                                    ImGui::InputTextMultiline("##output", info_log, sizeof(info_log), ImVec2(-1.0f, 2 * ImGui::GetTextLineHeightWithSpacing()), ImGuiInputTextFlags_ReadOnly);


                            }
                            ImGui::PopID();
                            ImGui::Unindent();
                        }
                    }
                    ImGui::Unindent();
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Textures"))
        {
            for (const auto& texture : g_texture_table)
            {
                char buffer[128];
                std::sprintf(buffer, "Handle: %d", texture.handle);
                if (ImGui::CollapsingHeader(buffer))
                {
                    std::sprintf(buffer, "Texture id: %u, width: %d, height: %d, filepath: %s",
                                 texture.texture_id, texture.width, texture.height, texture.filepath.c_str());

                    if (ImGui::TreeNode(buffer))
                    {
                        // Need to find a good way to flip the image since ImGUI does not take in an image in bytes.
                        ImGui::Image((void*)texture.texture_id, ImVec2(texture.width / 4, texture.height / 4));
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Settings"))
        {
            static bool mode = false;
            if (ImGui::Checkbox("Draw polygon mode", &mode))
                glPolygonMode(GL_FRONT_AND_BACK, (mode) ? GL_LINE : GL_FILL);

            ImGui::EndTabItem();
        }

        // TODO: Look into: http://docs.gl/gl4/glGetDebugMessageLog

        ImGui::EndTabBar();
    }
}


