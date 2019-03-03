#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vge_gfx_gl.h>

namespace VGE
{
    struct OBJAsset
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv_coords;
        std::vector<glm::vec3> normals;
        std::vector<int> indices;
    };

    OBJAsset
    LoadOBJ(const char* filepath);
} // namespace vge
