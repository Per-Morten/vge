#include <vge_core.h>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

/// TODO: Refactor into vge::gfx namespace
namespace VGE
{
    // Make it a struct or class, rather than namespace...
    // I do actually hold some data. And might want to expose that for testing purposes.
    struct GFXManager
    {
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

        static constexpr auto DynamicVBOSize = (1 << 21); // 2 MB

    };

    inline GFXManager gGfxManager;
}
