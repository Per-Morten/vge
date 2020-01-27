#pragma once
#include <vge_gfx_manager.h>
#include <vge_gfx_gl.h>
#include <vge_color.h>
#include <vge_draw_cmd.h>
#include <vge_gfx_types.h>


// #include <tuple>
// #include <glm/glm.hpp>
// // Stolen from the graphicsProgramming labs
// inline decltype(auto) CreateCube()
// {
//     using MeshInfo = std::tuple < std::vector<glm::vec3>
//         , std::vector<glm::vec3>
//         , std::vector<glm::vec2>
//         , std::vector < GLuint >> ;

//     std::vector<glm::vec3> cubePositions =
//     {
//         // Top
//         glm::vec3(-0.5f, -0.5f, 0.5f),
//         glm::vec3(0.5f, -0.5f, 0.5f),
//         glm::vec3(0.5f, 0.5f, 0.5f),
//         glm::vec3(-0.5f, 0.5f, 0.5f),

//         //right
//         glm::vec3(0.5f, -0.5f, 0.5f),
//         glm::vec3(0.5f, -0.5f, -0.5f),
//         glm::vec3(0.5f, 0.5f, -0.5f),
//         glm::vec3(0.5f, 0.5f, 0.5f),

//         //back
//         glm::vec3(0.5f, -0.5f, -0.5f),
//         glm::vec3(-0.5f, -0.5f, -0.5f),
//         glm::vec3(-0.5f, 0.5f, -0.5f),
//         glm::vec3(0.5f, 0.5f, -0.5f),

//         //left
//         glm::vec3(-0.5f, -0.5f, -0.5f),
//         glm::vec3(-0.5f, -0.5f, 0.5f),
//         glm::vec3(-0.5f, 0.5f, 0.5f),
//         glm::vec3(-0.5f, 0.5f, -0.5f),

//         //top
//         glm::vec3(-0.5f, 0.5f, 0.5f),
//         glm::vec3(0.5f, 0.5f, 0.5f),
//         glm::vec3(0.5f, 0.5f, -0.5f),
//         glm::vec3(-0.5f, 0.5f, -0.5f),

//         //bottom
//         glm::vec3(-0.5f, -0.5f, -0.5f),
//         glm::vec3(0.5f, -0.5f, -0.5f),
//         glm::vec3(0.5f, -0.5f, 0.5f),
//         glm::vec3(-0.5f, -0.5f, 0.5f)
//     };

//     std::vector<glm::vec3> cubeNormals =
//     {
//         ////////////////////////////////////
//         //front
//         glm::vec3(0.0, 0.0, 1.0),
//         glm::vec3(0.0, 0.0, 1.0),
//         glm::vec3(0.0, 0.0, 1.0),
//         glm::vec3(0.0, 0.0, 1.0),

//         //right
//         glm::vec3(1.0, 0.0, 0.0),
//         glm::vec3(1.0, 0.0, 0.0),
//         glm::vec3(1.0, 0.0, 0.0),
//         glm::vec3(1.0, 0.0, 0.0),

//         //back
//         glm::vec3(0.0, 0.0, -1.0),
//         glm::vec3(0.0, 0.0, -1.0),
//         glm::vec3(0.0, 0.0, -1.0),
//         glm::vec3(0.0, 0.0, -1.0),

//         //left
//         glm::vec3(-1.0, 0.0, 0.0),
//         glm::vec3(-1.0, 0.0, 0.0),
//         glm::vec3(-1.0, 0.0, 0.0),
//         glm::vec3(-1.0, 0.0, 0.0),

//         //top
//         glm::vec3(0.0, 1.0, 0.0),
//         glm::vec3(0.0, 1.0, 0.0),
//         glm::vec3(0.0, 1.0, 0.0),
//         glm::vec3(0.0, 1.0, 0.0),

//         //bottom
//         glm::vec3(0.0, -1.0, 0.0),
//         glm::vec3(0.0, -1.0, 0.0),
//         glm::vec3(0.0, -1.0, 0.0),
//         glm::vec3(0.0, -1.0, 0.0)
//     };

//     std::vector<glm::vec2> cubeTextureCoordinates =
//     {
//         //front
//         glm::vec2(0.0f, 0.0f),
//         glm::vec2(1.0f, 0.0f),
//         glm::vec2(1.0f, 1.0f),
//         glm::vec2(0.0f, 1.0f),

//         //right
//         glm::vec2(0.0f, 0.0f),
//         glm::vec2(1.0f, 0.0f),
//         glm::vec2(1.0f, 1.0f),
//         glm::vec2(0.0f, 1.0f),

//         //back
//         glm::vec2(0.0f, 0.0f),
//         glm::vec2(1.0f, 0.0f),
//         glm::vec2(1.0f, 1.0f),
//         glm::vec2(0.0f, 1.0f),

//         //left
//         glm::vec2(0.0f, 0.0f),
//         glm::vec2(1.0f, 0.0f),
//         glm::vec2(1.0f, 1.0f),
//         glm::vec2(0.0f, 1.0f),

//         //top
//         glm::vec2(0.0f, 0.0f),
//         glm::vec2(1.0f, 0.0f),
//         glm::vec2(1.0f, 1.0f),
//         glm::vec2(0.0f, 1.0f),

//         //bottom
//         glm::vec2(0.0f, 0.0f),
//         glm::vec2(1.0f, 0.0f),
//         glm::vec2(1.0f, 1.0f),
//         glm::vec2(0.0f, 1.0f)
//     };

//     std::vector<GLuint> IBO =
//     {
//         //front
//         0,1,2, 0,2,3,

//         //right
//         4,5,6, 4,6,7,

//         //back
//         8,9,10, 8,10,11,

//         //left
//         12,13,14, 12,14,15,

//         //top
//         16,17,18, 16,18,19,

//         //bottom
//         20,21,22, 20,22,23
//     };

//     MeshInfo result = std::make_tuple(cubePositions, cubeNormals, cubeTextureCoordinates, IBO);
//     return result;
// }
