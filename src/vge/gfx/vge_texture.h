#pragma once
#include <vge_core.h>
#include <glad/glad.h>


namespace vge
{
    struct texture
    {
        using texture_id = GLuint;

        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        texture(texture&& other) VGE_NOEXCEPT;
        texture& operator=(texture&& other) VGE_NOEXCEPT;

        ~texture() VGE_NOEXCEPT;

        texture(const char* texture_path) VGE_NOEXCEPT;

        void bind(GLenum texture_unit) VGE_NOEXCEPT;

        texture_id handle;
    };
}
