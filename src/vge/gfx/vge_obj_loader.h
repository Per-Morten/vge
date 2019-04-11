#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vge_gfx_gl.h>
#include <vge_array.h>

namespace VGE
{
    struct OBJAsset
    {
        VGE::Array<glm::vec3> positions;
        VGE::Array<glm::vec2> uv_coords;
        VGE::Array<glm::vec3> normals;
        VGE::Array<int> indices;
    };

    OBJAsset
    LoadOBJ(const char* filepath);
} // namespace vge
