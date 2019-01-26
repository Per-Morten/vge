#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <vge_core.h>
#include <vge_memory.h>
#include <vge_profiler.h>
#include <vge_gfx.h>
#include <vge_debug.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void
framebuffer_size_callback([[maybe_unused]] GLFWwindow* window,
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
main(int argc, char** argv)
{
    vge::initialize_logger();

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
        return ((vge::malloc_allocator*)allocator)->allocate(size);
    };
    auto imgui_dealloc = [](void* ptr, void* allocator)
    {
        return ((vge::malloc_allocator*)allocator)->deallocate(ptr);
    };

    VGE_DEBUG("%p", &imgui_dealloc);
    vge::malloc_allocator imgui_allocator("imgui");
    ImGui::SetAllocatorFunctions(imgui_alloc, imgui_dealloc, &imgui_allocator);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    vge::init_imgui_style();

    // Setup subsystems
    vge::malloc_allocator profiler_allocator("profiler");
    vge::initialize_profiler(profiler_allocator);

    // Toying with OpenGL
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    glm::vec3 vertices[] = {
        {0.5f,  0.5f, 0.0f},  // top right
        {0.5f, -0.5f, 0.0f},  // bottom right
        {-0.5f, -0.5f, 0.0f},  // bottom left
        {-0.5f,  0.5f, 0.0f},  // top left
    };

    glm::vec2 tex_coords[] = {
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f},
    };

    vge::gfx_manager::mesh_data data;
    data.name = "test";
    data.vertices = vertices;
    data.uv0 = tex_coords;
    data.triangles = indices;
    data.triangle_count = 6;
    data.vertex_count = 4;

    auto handle = vge::gfx_manager::create_mesh();
    vge::gfx_manager::set_mesh(handle, data);
    auto shader_handle = vge::gfx_manager::create_shader();
    vge::gfx_manager::attach_shader(shader_handle, "resources/shaders/basic_shader.vs", GL_VERTEX_SHADER);
    vge::gfx_manager::attach_shader(shader_handle, "resources/shaders/basic_shader.fs", GL_FRAGMENT_SHADER);
    vge::gfx_manager::compile_and_link_shader(shader_handle);
    auto shader_id = vge::gfx_manager::get_shader_id(shader_handle);

    glUseProgram(shader_id);

    auto tex_handle1 = vge::gfx_manager::create_texture();
    vge::gfx_manager::load_texture(tex_handle1, "resources/textures/container.jpg");
    auto tex_handle2 = vge::gfx_manager::create_texture();
    vge::gfx_manager::load_texture(tex_handle2, "resources/textures/awesomeface.png");

    glUniform1i(glGetUniformLocation(shader_id, "u_texture0"), 0);
    glUniform1i(glGetUniformLocation(shader_id, "u_texture1"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vge::gfx_manager::get_texture_id(tex_handle1));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vge::gfx_manager::get_texture_id(tex_handle2));

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // New Frame
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clearing
        const auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // Drawing
        vge::gfx_manager::draw_mesh(handle);

        // Draw subsystems
        vge::draw_debug_windows();

        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Subsystem shutdown
    vge::shutdown_profiler();

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
