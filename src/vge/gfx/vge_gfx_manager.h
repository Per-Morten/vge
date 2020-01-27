#pragma once
#include <vge_core.h>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vge_color.h>
#include <vge_draw_cmd.h>
#include <vge_gfx_types.h>

namespace VGE
{
    // Make it a struct or class, rather than namespace...
    // I do actually hold some data. And might want to expose that for testing purposes.
    struct GFXManager
    {
        void Init();

        // Mesh Related
        MeshHandle CreateMesh();
        void DestroyMesh(MeshHandle handle);
        void SetMesh(MeshHandle handle, MeshData data);
        void DrawMesh(MeshHandle handle);
        // TODO: Need a way to get access to the different buffers on the GPU so I can directly map and work on them

        // Texture Related
        // TODO: Figure out this interface
        TextureHandle CreateTexture();
        void LoadTexture(TextureHandle handle, const char* filepath);
        TextureID GetTextureID(TextureHandle handle);

        // Shader related
        // TODO: Figure out this interface
        ShaderHandle CreateShader();
        void AttachShader(ShaderHandle handle, const char* filepath, GLenum type);
        void CompileAndLinkShader(ShaderHandle handle);
        ProgramID GetShaderID(ShaderHandle handle);

        // Debugging
        void DrawDebug();

        // Should have some sort of shutdown function?

        // TOOD: Need to create some sort of "immediate mode" layer. For directly drawing vertices.
        GLuint mDynamicVBO;
        GLuint mDynamicVAO;

        struct Vertex
        {
            glm::vec3 position;
            glm::vec4 color;
        };

        static constexpr auto DynamicVBOSize = (1 << 21); // 2 MB
        static constexpr auto MaxDynamicVertices = DynamicVBOSize / sizeof(Vertex);

        Vertex mDynamicVertices[MaxDynamicVertices];
        int mDynamicVerticesCount = 0;


        // TODO: Move to a commit based system.
        // You can commit mesh drawings, or dynamic vertices.
        // You also commit the shaders and settings you want to use.
        void DrawLine(glm::vec3 begin, glm::vec3 end, Color color);
        void RenderImmediate();

        void SubmitStaticDrawCommand(const StaticDrawCommand& command);

        // Will start thread in future. After this point, don't submit more static draw commands!
        void RenderStatic();

        // Temporary solution
        static constexpr auto MaxStaticDrawCommands = 2048;
        StaticDrawCommand mStaticCommands[MaxStaticDrawCommands];
        int mStaticCommandsCount = 0;

    };

    inline GFXManager gGfxManager;
}
