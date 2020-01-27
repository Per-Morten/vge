#pragma once
#include <vge_assert.h>
#include <vge_imgui.h>
#include <vge_log.h>
#include <vge_profiler.h>
#include <vge_memory.h>
#include <vge_gfx.h>

// Move stuff into a Debug Class.
// Basically just so that I can write tests etc.
namespace VGE
{
    struct Debug
    {
        void Init();

        void DrawLine(glm::vec3 begin, glm::vec3 end, Color color = Color::White);
        void DrawRay(glm::vec3 origin, glm::vec3 direction, Color color = Color::White);
        void DrawAxes(glm::vec3 center);
        void DrawCircle(glm::vec3 center, float radius, Color color = Color::White);
        // Need a draw arc

        void RenderDebugLines(glm::mat4 view_projection_matrix);

        ShaderHandle mProgramHandle;
    };

    inline Debug gDebug;
}
