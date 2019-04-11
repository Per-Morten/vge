#include <imgui.h>
#include <vge_imgui.h>
#include <vge_gfx_manager.h>
#include <vge_log.h>
#include <vge_memory.h>
#include <vge_profiler.h>

void
vge::init_imgui_style()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.725f));
}

void
vge::draw_debug_windows()
{
    struct subsystem
    {
        const char* name;
        void (*function)();
        bool enabled;
    };

    static subsystem systems[] =
    {
        {"Graphics", []{VGE::gGfxManager.DrawDebug();}, false},
        {"Extended Logging", draw_log_window, false},
        {"Memory", []{VGE::gMemoryManager.DrawDebug();}, true},
        {"Profiler", []{VGE::gProfiler.DrawProfiler();}, true},
    };

    static bool overlay = true;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Overlay", nullptr, &overlay);

            for (auto& item : systems)
                ImGui::MenuItem(item.name, nullptr, &item.enabled);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (overlay)
    {
        static constexpr int lines_in_overlay = 10;
        ImGui::SetNextWindowBgAlpha(0.3f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, ImGui::GetTextLineHeight() * (lines_in_overlay + 0.5f)));

        if (ImGui::Begin("##overlay", nullptr,
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
                        | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoScrollbar
                        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Columns(2, nullptr, true);
            ImGui::SetColumnWidth(-1, io.DisplaySize.x / 4);

            if (ImGui::BeginChild(ImGui::GetID("fps_overlay"), ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMouseInputs))
                ImGui::Text("fps: %.1f\navg: %.3f ms/frame", io.Framerate, 1000.0f / io.Framerate);

            ImGui::EndChild();
            ImGui::NextColumn();

            draw_log_overlay(lines_in_overlay);
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
    }


    for (auto& item : systems)
    {
        if (item.enabled)
        {
            if (ImGui::Begin(item.name, &item.enabled))
                item.function();
            ImGui::End();
        }
    }
}
