#pragma once
#include <vge_core.h>
#include <GL/glew.h>

namespace vge
{
    /// TODO:
    /// Modify interface so we can also create other shader combinations by attaching shaders, and manually linking this.
    struct shader
    {
        using program_id = GLuint;

        shader(const shader&) = delete;
        shader& operator=(const shader&) = delete;

        shader(shader&& other) VGE_NOEXCEPT;
        shader& operator=(shader&& other) VGE_NOEXCEPT;

        ~shader() VGE_NOEXCEPT;

        shader(const char* vertex_path, const char* fragment_path) VGE_NOEXCEPT;

        void bind();

        program_id handle;
    };
}
