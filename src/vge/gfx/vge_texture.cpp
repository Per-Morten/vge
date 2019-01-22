#include <vge_texture.h>
#include <stb_image.h>

vge::texture::texture(const char* texture_path) VGE_NOEXCEPT
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);
    auto data = stbi_load(texture_path, &width, &height, &channels, 0);
    if (!data)
    {
        VGE_ERROR("Could not load image: %s, %s", texture_path, stbi_failure_reason());
    }

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const auto fmt = (channels < 4) ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);


    stbi_image_free(data);

}

vge::texture::texture(texture&& other) VGE_NOEXCEPT
{
    handle = other.handle;
    other.handle = 0;
}

vge::texture&
vge::texture::operator=(texture&& other) VGE_NOEXCEPT
{
    if (this != &other)
    {
        glDeleteTextures(1, &handle);
        handle = other.handle;
        other.handle = 0;
    }
    return *this;
}

vge::texture::~texture() VGE_NOEXCEPT
{
    glDeleteTextures(1, &handle);
}

void
vge::texture::bind(GLenum texture_unit) VGE_NOEXCEPT
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, handle);
}
