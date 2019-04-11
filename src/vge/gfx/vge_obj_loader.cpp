#include <vge_obj_loader.h>
#include <vge_utility.h>
#include <vge_debug.h>
#include <tiny_obj_loader.h>

// Just use TinyOBJ loader, and look at this: https://vulkan-tutorial.com/Loading_models
VGE::OBJAsset
VGE::LoadOBJ(const char* filepath)
{
    // At a quick glance there doesn't seem to be anything in tinyobj that stops it from being able to be parallelized.
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    OBJAsset asset;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath))
        VGE_ERROR("Could not load obj, err: %s, warn: %s", err.c_str(), warn.c_str());

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            asset.positions.PushBack({
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            });

            asset.uv_coords.PushBack({
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            });

            asset.normals.PushBack({
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            });

            asset.indices.PushBack(asset.indices.Size());
        }
    }

    return asset;
}


