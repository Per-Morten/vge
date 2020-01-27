#pragma once
#include <vge_gfx_types.h>
#include <vge_debug.h>

namespace VGE
{
    struct Uniform
    {
        Uniform()
        {}

        Uniform(const char* name)
        {
            VGE_ASSERT(std::strlen(name) < sizeof(Name), "Uniform name: %s is longer than %zu", std::strlen(name), sizeof(Name));
            std::strcpy(Name, name);
        }

        char Name[64]{};

        enum UniformType
        {
            Int,
            Float,
            Vec2,
            Vec3,
            Vec4,
            Mat4,
            Sampler2D,
        };

        UniformType Type;

        union
        {
            int AsInt;
            float AsFloat;
            glm::vec2 AsVec2;
            glm::vec3 AsVec3;
            glm::vec4 AsVec4;
            glm::mat4 AsMat4;
        };
    };

    // At the moment, only support 2 textures
    struct StaticDrawCommand
    {
        MeshHandle Mesh{};
        ShaderHandle Shader{};
        TextureHandle UV0{};
        TextureHandle UV1{};

        Uniform Uniforms[16];
        int UniformCount{};
    };

    // This need to be extended to also allow for triangles.
    // Currently only lines is supported.
    struct DynamicDrawCommand
    {
        ShaderHandle Shader{};
        glm::vec3* Vertices{};
        Color* Colors{};
        int VerticesCount{};

        Uniform Uniforms[16];
        int UniformCount{};
    };


        // TYPE: Dynamic, Static
        // - Dynamic
        //      - Vertices
        //      - ShaderProgram
        //      - Uniform Configs
        // - Static
        //      - Mesh Resource ID
        //      - ShaderProgram
        //      - Uniform Configs


}
