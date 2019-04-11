#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <vge_core.h>
#include <vge_memory.h>
#include <vge_profiler.h>
#include <vge_gfx.h>
#include <vge_debug.h>
#include <vge_obj_loader.h>
#include <vge_array.h>
#include <vge_slot_map.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <tiny_obj_loader/tiny_obj_loader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void
framebuffer_size_callback(VGE_UNUSED GLFWwindow* window,
                          int width, int height)
{
    glViewport(0, 0, width, height);
}

void
process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int
main(VGE_UNUSED int argc,
     VGE_UNUSED char** argv)
{
    using namespace VGE;

    vge::init_logger();

    if (!glfwInit())
    {
        glfwTerminate();
        VGE_ERROR("Failed to init GLFW");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 640, "vge", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (window == nullptr)
    {
        glfwTerminate();
        VGE_ERROR("Failed to create window");
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        VGE_ERROR("Failed to initialize GLAD");
    }

    glViewport(0, 0, 800, 640);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto imgui_alloc = [](std::size_t size, void* allocator)
    {
        return ((VGE::MallocAllocator*)allocator)->Allocate(size);
    };
    auto imgui_dealloc = [](void* ptr, void* allocator)
    {
        return ((VGE::MallocAllocator*)allocator)->Deallocate(ptr);
    };

    VGE::MallocAllocator imgui_allocator("imgui");
    ImGui::SetAllocatorFunctions(imgui_alloc, imgui_dealloc, &imgui_allocator);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    vge::init_imgui_style();
    vge::init_gl_logger();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Setup subsystems
    gGfxManager.Init();


    auto shader_handle = gGfxManager.CreateShader();

    gGfxManager.AttachShader(shader_handle, "resources/shaders/basic_shader.vs", GL_VERTEX_SHADER);
    gGfxManager.AttachShader(shader_handle, "resources/shaders/basic_shader.fs", GL_FRAGMENT_SHADER);
    gGfxManager.CompileAndLinkShader(shader_handle);
    auto shader_id = gGfxManager.GetShaderID(shader_handle);


    auto tex_handle1 = gGfxManager.CreateTexture();
    gGfxManager.LoadTexture(tex_handle1, "resources/textures/container.jpg");
    auto tex_handle2 = gGfxManager.CreateTexture();
    gGfxManager.LoadTexture(tex_handle2, "resources/textures/awesomeface.png");

    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, "u_texture0"), 0);
    glUniform1i(glGetUniformLocation(shader_id, "u_texture1"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gGfxManager.GetTextureID(tex_handle1));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gGfxManager.GetTextureID(tex_handle2));

    auto object = VGE::LoadOBJ("resources/meshes/cube/cube.obj");
    auto handle2 = gGfxManager.CreateMesh();
    VGE::GFXManager::MeshData data2;
    data2.name = "obj_file";
    data2.triangles = (GLuint*)object.indices.Data();
    data2.triangle_count = object.indices.Size();
    data2.vertex_count = object.positions.Size();
    data2.vertices = object.positions.Data();
    data2.uv0 = object.uv_coords.Data();
    gGfxManager.SetMesh(handle2, data2);

    auto third = gGfxManager.CreateMesh();
    gGfxManager.SetMesh(third, data2);

    glm::mat4 view          = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 projection    = glm::mat4(1.0f);

    // near = 0.1, far = 100.0f
    projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)640, 0.1f, 100.0f);
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader_id, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // New Frame
        VGE::gProfiler.BeginFrame();
        {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Clearing
            const auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniformMatrix4fv(glGetUniformLocation(shader_id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            glm::mat4 model = glm::mat4(1.0f);
            // pass transformation matrices to the shader

            glUniformMatrix4fv(glGetUniformLocation(shader_id, "model"), 1, GL_FALSE, glm::value_ptr(model));

            // Drawing
            gGfxManager.DrawMesh(handle2);

            model = glm::translate(glm::mat4(1.0f) ,glm::vec3(1.0f,  1.0f, -1.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader_id, "model"), 1, GL_FALSE, glm::value_ptr(model));
            gGfxManager.DrawMesh(handle2);
            model = glm::translate(glm::mat4(1.0f) ,glm::vec3(-1.0f,  1.0f, -1.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader_id, "model"), 1, GL_FALSE, glm::value_ptr(model));
            gGfxManager.DrawMesh(handle2);
            model = glm::translate(glm::mat4(1.0f) ,glm::vec3(1.0f,  -1.0f, -1.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader_id, "model"), 1, GL_FALSE, glm::value_ptr(model));
            gGfxManager.DrawMesh(handle2);
            model = glm::translate(glm::mat4(1.0f) ,glm::vec3(-1.0f,  -1.0f, -1.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader_id, "model"), 1, GL_FALSE, glm::value_ptr(model));
            gGfxManager.DrawMesh(handle2);

            // Draw subsystems
        }

        vge::draw_debug_windows();

        //ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Subsystem shutdown

    //
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
