#include <vge_debug.h>
#include <glm/gtc/type_ptr.hpp>

void
VGE::Debug::Init()
{
    mProgramHandle = gGfxManager.CreateShader();
    gGfxManager.AttachShader(mProgramHandle, "resources/shaders/debug_shader.vs", GL_VERTEX_SHADER);
    gGfxManager.AttachShader(mProgramHandle, "resources/shaders/debug_shader.fs", GL_FRAGMENT_SHADER);
    gGfxManager.CompileAndLinkShader(mProgramHandle);
}

void
VGE::Debug::DrawLine(glm::vec3 begin, glm::vec3 end, Color color)
{
    gGfxManager.DrawLine(begin, end, color);
}

void
VGE::Debug::DrawRay(glm::vec3 origin, glm::vec3 direction, Color color)
{

}

void
VGE::Debug::DrawAxes(glm::vec3 center)
{
    gGfxManager.DrawLine(center, center + glm::vec3(1.0f, 0.0f, 0.0f), Color::Red);
    gGfxManager.DrawLine(center, center + glm::vec3(0.0f, 1.0f, 0.0f), Color::Green);
    gGfxManager.DrawLine(center, center + glm::vec3(0.0f, 0.0f, 1.0f), Color::Blue);
}

void
VGE::Debug::DrawCircle(glm::vec3 center, float radius, Color color)
{

}

void
VGE::Debug::RenderDebugLines(glm::mat4 view_project_matrix)
{
    auto debug_id = gGfxManager.GetShaderID(mProgramHandle);
    glProgramUniformMatrix4fv(debug_id, glGetUniformLocation(debug_id, "uVP"), 1, GL_FALSE, glm::value_ptr(view_project_matrix));
    glUseProgram(debug_id);
    gGfxManager.RenderImmediate();
}

