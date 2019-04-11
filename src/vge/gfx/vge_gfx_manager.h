#include <vge_core.h>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

/// TODO: Refactor into vge::gfx namespace
namespace vge
{
    // Should just be namespace, not a "class"
    namespace gfx_manager
    {
        struct mesh_data
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

        // Mesh Related
        MeshHandle create_mesh();
        void destroy_mesh(MeshHandle handle);
        void set_mesh(MeshHandle handle, mesh_data data);
        void draw_mesh(MeshHandle handle);
        // TODO: Need a way to get access to the different buffers on the GPU so I can directly map and work on them

        // Texture Related
        // TODO: Figure out this interface
        TextureHandle create_texture();
        void load_texture(TextureHandle handle, const char* filepath);
        TextureID get_texture_id(TextureHandle handle);

        // Shader related
        // TODO: Figure out this interface
        ShaderHandle create_shader();
        void attach_shader(ShaderHandle handle, const char* filepath, GLenum type);
        void compile_and_link_shader(ShaderHandle handle);
        ProgramID get_shader_id(ShaderHandle handle);

        // Utility
        void draw_imgui_debug();

        // Should have some sort of shutdown function?
    };
}
