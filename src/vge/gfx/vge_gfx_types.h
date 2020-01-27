#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace VGE
{
    // Should have colors as well!
    // TODO: Rename to StaticMeshData
    struct MeshData
    {
        std::string name;
        int vertex_count;
        int triangle_count;
        glm::vec3* vertices{};
        GLuint* triangles{};
        glm::vec2* uv0{};
        glm::vec2* uv1{};
    };
    // TODO: Create generational handle to be used here.
    // Will both help detecting "invalid references", and can also help making the system more typesafe.
    using MeshHandle = int;
    using ShaderHandle = int;
    using TextureHandle = int;
    using ProgramID = GLuint;
    using ShaderID = GLuint;
    using TextureID = GLuint;
}
