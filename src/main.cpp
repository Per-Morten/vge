#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <vge_core.h>
#include <vge_memory.h>
#include <vge_profiler.h>
#include <vge_gfx.h>

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
    vge::malloc_allocator imgui_allocator("imgui");
    ImGui::SetAllocatorFunctions(imgui_alloc, imgui_dealloc, &imgui_allocator);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    // Setup subsystems
    vge::malloc_allocator profiler_allocator("profiler");
    vge::initialize_profiler(profiler_allocator);

    // Toying with OpenGL
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    float tex_coords[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };

    glm::vec3 vertices2[] = {
        {0.5f,  0.5f, 0.0f},  // top right
        {0.5f, -0.5f, 0.0f},  // bottom right
        {-0.5f, -0.5f, 0.0f},  // bottom left
        {-0.5f,  0.5f, 0.0f},  // top left
    };

    glm::vec2 tex_coords2[] = {
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f},
    };

    auto manager = vge::gfx_manager();
    vge::gfx_manager::mesh_data data;
    data.name = "test";
    data.vertices = vertices2;
    data.uv0 = tex_coords2;
    data.triangles = indices;
    data.triangle_count = 6;
    data.vertex_count = 4;

    auto handle = manager.create_mesh();
    manager.set_mesh(handle, data);
    auto shader_handle = manager.create_shader();
    manager.attach_shader(shader_handle, "resources/shaders/basic_shader.vs", GL_VERTEX_SHADER);
    manager.attach_shader(shader_handle, "resources/shaders/basic_shader.fs", GL_FRAGMENT_SHADER);
    manager.compile_and_link_shader(shader_handle);
    auto shader_id = manager.get_shader_id(shader_handle);

    glUseProgram(shader_id);
    //auto texture1 = vge::texture("resources/textures/container.jpg");
    //auto texture2 = vge::texture("resources/textures/awesomeface.png");

    auto tex_handle1 = manager.create_texture();
    manager.load_texture(tex_handle1, "resources/textures/container.jpg");
    auto tex_handle2 = manager.create_texture();
    manager.load_texture(tex_handle2, "resources/textures/awesomeface.png");

    // Draw Wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Regular drawing:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //shader.bind();
    glUniform1i(glGetUniformLocation(shader_id, "u_texture0"), 0);
    glUniform1i(glGetUniformLocation(shader_id, "u_texture1"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manager.get_texture_id(tex_handle1));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, manager.get_texture_id(tex_handle2));
    // texture1.bind(GL_TEXTURE0);
    // texture2.bind(GL_TEXTURE1);

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
        //const auto clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // Drawing
        manager.draw_mesh(handle);

        // Draw subsystems
        //vge::draw_profiler();
        manager.draw_imgui_debug();
        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //glfwMakeContextCurrent(window);
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
