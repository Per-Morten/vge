#include <algorithm>
#include <cstdio>


#include <vge_core.h>
#include <vge_memory.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void
imgui_test()
{
    ImGui::ShowDemoWindow();
    ImGui::Begin("Profiler");
    auto prof_list = ImGui::GetWindowDrawList();
    prof_list->AddRectFilled(ImVec2(100.0f, 100.0f), ImVec2(2.0f, 2.0f), 0x33FFFFFF);

    if (ImGui::TreeNode("Node"))
    {
        ImGui::TreePop();
    }

    ImGui::End();
}

int
main(int argc, char** argv)
{
    VGE_INFO("After init");
    if (!glfwInit())
    {
        glfwTerminate();
        VGE_ERROR("Failed to init GLFW");
    }


    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 640, "vge", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (window == nullptr)
    {
        glfwTerminate();
        VGE_ERROR("Failed to create window");
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        VGE_ERROR("Failed to init GLEW");
    }

    VGE_DEBUG("Before IMGUI");

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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        imgui_test();

        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        const auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}
