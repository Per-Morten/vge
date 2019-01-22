#include <vge_core.h>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

/// TODO: Refactor into vge::gfx namespace
namespace vge
{
    // Should just be namespace, not a "class"
    struct gfx_manager
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
        using mesh_handle = int;
        using shader_handle = int;
        using texture_handle = int;
        using program_id = GLuint;
        using shader_id = GLuint;
        using texture_id = GLuint;

        // Mesh Related
        mesh_handle create_mesh();
        void destroy_mesh(mesh_handle handle);
        void set_mesh(mesh_handle handle, mesh_data data);
        void draw_mesh(mesh_handle handle);
        // TODO: Need a way to get access to the different buffers on the GPU so I can directly map and work on them

        // Texture Related
        // TODO: Figure out this interface
        texture_handle create_texture();
        void load_texture(texture_handle handle, const char* filepath);
        texture_id get_texture_id(texture_handle handle);

        // Shader related
        // TODO: Figure out this interface
        shader_handle create_shader();
        void attach_shader(shader_handle handle, const char* filepath, GLenum type);
        void compile_and_link_shader(shader_handle handle);
        program_id get_shader_id(shader_handle handle);

        // Utility
        void draw_imgui_debug();

        // Should have some sort of shutdown function?
    };
}
