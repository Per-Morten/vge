#include <imgui.h>
#include <vge_profiler.h>
#include <vge_assert.h>

void
VGE::Profiler::PushProfileEvent(const VGE::ProfileEvent& event)
{
    auto thread_id = VGE::Thread::ThisThread::ID();
    auto& curr_count = mEventsCount[thread_id];
    VGE_ASSERT(curr_count < MaxEvents, "Trying to push more events than there are room for!");

    mEvents[thread_id][curr_count++] = event;
}

void
VGE::Profiler::BeginFrame()
{
    for (int i = 0; i < std::size(mEventsCount); i++)
        mEventsCount[i] = 0;
    mFrameStart = std::chrono::high_resolution_clock::now();
}

void
VGE::Profiler::DrawProfiler()
{
    constexpr auto DurToMilli = [](ProfileTimePoint begin, ProfileTimePoint end)
    { return std::chrono::duration<float, std::chrono::milliseconds::period>(end - begin).count(); };

    const auto frame_stop = std::chrono::high_resolution_clock::now();
    const float window_width = ImGui::GetContentRegionAvailWidth();

    const auto frame_duration = DurToMilli(mFrameStart, frame_stop);
    const float milli_pr_pixel = frame_duration / window_width;
    ImGui::Text("frame_duration: %f, microsec_pr_pixel: %f", frame_duration, milli_pr_pixel);

    // First draw all of the frame times.



    // Then, draw all timelines in collapsable headers.
    auto list = ImGui::GetWindowDrawList();
    const auto window_x_begin = ImGui::GetCursorScreenPos().x;

    for (int t = 0; t < VGE::Thread::MaxThreads; t++)
    {
        char buffer[64];
        std::sprintf(buffer, "ThreadID: %d", t);
        if (ImGui::CollapsingHeader(buffer, ImGuiTreeNodeFlags_DefaultOpen))
        {
            const auto text_height = ImGui::GetTextLineHeightWithSpacing();
            // Draw Whole duration
            {
                const auto x_begin = window_x_begin;
                const auto x_end = window_x_begin + (frame_duration / milli_pr_pixel);
                const auto y_begin = ImGui::GetCursorScreenPos().y;
                const auto y_end = y_begin + text_height;


                list->AddRectFilled({x_begin, y_begin}, {x_end, y_end}, 0xFFFFFFFF);
                list->AddRect({x_begin, y_begin}, {x_end, y_end}, 0xFF << 24);
            }

            constexpr static ImU32 colors[] =
            {
                0xFF555555,
                0x1abc9c,
                0x27ae60,
                0x3498db,
                0xe74c3c,
                0xf39c12,
                0x6bb2d1,
                0x518692,
                0x225656,
            };

            auto window_begin = ImGui::GetCursorScreenPos().y;

            ProfileTimePoint stack[256];
            stack[0] = mFrameStart;
            int layer = 0;

            for (int i = mEventsCount[t] - 1, c = 0; i >= 0; i--, c = (c + 1) % (sizeof(colors) / sizeof(colors[0])))
            {
                // Find correct layer to put event on.
                while (mEvents[t][i].Begin < stack[layer])
                    layer--;

                if (mEvents[t][i].Begin > stack[layer])
                    layer++;
                stack[layer] = mEvents[t][i].Begin;

                VGE_ASSERT(layer < 256, "Callstack to deep, or malfunction in layering algorithm");

                // Draw the events
                const auto x_begin = window_x_begin + (DurToMilli(mFrameStart, mEvents[t][i].Begin) / milli_pr_pixel);
                const auto x_end = window_x_begin + (DurToMilli(mFrameStart, mEvents[t][i].End) / milli_pr_pixel);
                const auto y_begin = window_begin + (text_height * (layer - 1));
                const auto y_end = y_begin + text_height;

                list->AddRectFilled({x_begin, y_begin}, {x_end, y_end}, colors[c] | (0xFF << 24));
                list->AddRect({x_begin, y_begin}, {x_end, y_end}, 0xFF << 24);

                ImGui::SetCursorScreenPos({x_begin, y_begin});
                ImGui::Dummy({x_end - x_begin, y_end - y_begin});
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();

                    const auto duration = DurToMilli(mEvents[t][i].Begin, mEvents[t][i].End);
                    ImGui::Text("Event: %s\nDuration: %.4f (%.2f%%)",
                                mEvents[t][i].Label,
                                duration,
                                duration / frame_duration);

                    ImGui::EndTooltip();
                }
                ImGui::SetCursorScreenPos({x_begin, y_begin});
                ImGui::Text("%s", mEvents[t][i].Label);
            }
            ImGui::NewLine();
            ImGui::NewLine();
        }
    }



    // TODO: Find way to make it less Jittery?
    // TODO: Add button for "locking" frame-time
    // TODO: Display warning if exceeding frame-time when having locked frame-time.
    // TODO: Do one child window pr thread, and label them properly!

}
