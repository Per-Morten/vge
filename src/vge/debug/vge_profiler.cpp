#include <imgui.h>

#include <vge_profiler.h>


void
vge::initialize_profiler(vge::allocator& allocator) VGE_NOEXCEPT
{
    // Don't use allocator here, rather just have an internal large array?

}

void
vge::shutdown_profiler() VGE_NOEXCEPT
{

}

void
vge::push_profile_event(int thread_id, char* filename, char* function, int line, int duration) VGE_NOEXCEPT
{

}

void
vge::draw_profiler() VGE_NOEXCEPT
{
    ImGui::SetNextWindowPos({0.0f, 0.0f});
    ImGui::SetNextWindowSize({400.0f, 640.0f});
    ImGui::Begin("Profiler", NULL, ImGuiWindowFlags_NoBackground & ImGuiWindowFlags_None);


    ImGui::BeginChild("Test", ImVec2(0, 200.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
    auto prof_list = ImGui::GetWindowDrawList();

    bool hovered[4];

    for (int i = 0; i < 4; i++)
    {
        auto curr_pos = ImGui::GetCursorScreenPos();
        auto first_begin = ImVec2(curr_pos.x, curr_pos.y);
        auto first_end = ImVec2(curr_pos.x + 200, curr_pos.y + ImGui::GetTextLineHeightWithSpacing());

        auto style = ImGui::GetStyle();
        auto padding = style.WindowPadding;
        //auto graphics_begin = ImVec2(std::max(ImGui::GetWindowPos().x + padding.x, curr_pos.x), curr_pos.y);

        prof_list->AddRectFilled(first_begin, first_end, 0xFF0000FF);
        ImGui::SetCursorScreenPos(first_begin);
        ImGui::Dummy({first_end.x - first_begin.x, first_end.y - first_begin.y});
        hovered[i] = ImGui::IsItemHovered();



        ImGui::SetCursorScreenPos(first_begin);
        ImGui::Text("Text %d", i);

        ImGui::SetCursorScreenPos({first_end.x, first_begin.y});
    }


    ImGui::EndChild();

    ImGui::Begin("Debug##Default");
    for (int i = 0; i < 4; i++)
        ImGui::Value("Hovered", hovered[i]);
    //ImGui::Value("Hovered", hovered);
    //ImGui::Value("Hovered 2", hovered2);
    ImGui::End();

    ImGui::End();
}
