#include <vge_memory_manager.h>
#include <vge_assert.h>
#include <imgui.h>

void
VGE::MemoryManager::RegisterAllocator(Allocator* allocator)
{
    // Switch to binary search after verifying correctness, before doing binary search though, do check that we are not larger then the end, because in that case we can just append the end.
    // Sort not based on the address of the allocator, but rather the address of where the memory begins!
    auto& alloc_count = mAllocatorsCount[VGE::Thread::ThisThread::ID()];
    VGE_ASSERT(alloc_count < MaxAllocators, "We are either leaking allocators, or are trying to add more allocators than maximum 256");

    Allocator** allocators = mAllocators[VGE::Thread::ThisThread::ID()];

    int position = 0;
    while (position < alloc_count && allocator >= allocators[position])
        position++;

    // Shift all objects one space up
    std::memmove(&allocators[position + 1], &allocators[position], (alloc_count - position) * sizeof(VGE::Allocator*));
    allocators[position] = allocator;

    alloc_count++;
}

void
VGE::MemoryManager::DeregisterAllocator(Allocator* allocator)
{
    // Switch to binary search after verifying correctness.
    // Make the assumption here that allocators are short lived, and therefore I should search backwards
    auto& alloc_count = mAllocatorsCount[VGE::Thread::ThisThread::ID()];
    Allocator** allocators = mAllocators[VGE::Thread::ThisThread::ID()];

    int position = alloc_count - 1;
    while (position >= 0 && allocator < allocators[position])
        position--;

    VGE_ASSERT(position >= 0, "Trying to doubly delete allocator with name: %s", allocator->Name());

    std::memmove(&allocators[position], &allocators[position + 1], (alloc_count - position) * sizeof(VGE::Allocator*));
    alloc_count--;
}

void
VGE::MemoryManager::DrawDebug()
{
    // TODO: Need to find another reporting scheme as well, because this will not work for frame based allocators.
    if (ImGui::BeginTabBar("MemoryTab"))
    {
        if (ImGui::BeginTabItem("Memory View"))
        {
            // TODO: Make this more dynamic
            const auto minimap_width = 40.0f;
            const auto detailed_width = std::min((ImGui::GetContentRegionAvailWidth() * 0.4f) - minimap_width, 120.0f);
            const auto list_width = ImGui::GetContentRegionAvailWidth() - detailed_width - minimap_width;

            ImGui::BeginChild(ImGui::GetID("AllocatorList"), ImVec2(list_width, 0.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            {
                // TODO: Should make it clear that this is on a per thread basis.
                for (int t = 0; t != (int)std::size(mAllocatorsCount); t++)
                {
                    for (int i = 0; i != mAllocatorsCount[t]; i++)
                    {
                        if (ImGui::TreeNode((void*)(i + (t * MaxAllocators)), "Allocator: %s", mAllocators[t][i]->Name()))
                        {
                            ImGui::Text("Capacity: %d\nUsed: %d",
                                        mAllocators[t][i]->Capacity(),
                                        mAllocators[t][i]->AllocatedSize());

                            ImGui::TreePop();
                        }
                    }
                }
            }
            ImGui::EndChild();
            ImGui::SameLine();


            const auto DrawMemory = [&](int byte_pr_pixel)
            {
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

                const auto width = ImGui::GetContentRegionAvailWidth();
                const auto start_pos = ImGui::GetCursorScreenPos();
                const auto x_begin = ImGui::GetCursorScreenPos().x;
                const auto x_end = x_begin + width;

                auto list = ImGui::GetWindowDrawList();

                for (int t = 0; t != (int)std::size(mAllocatorsCount); t++)
                {
                    for (int i = 0, c = 0; i != mAllocatorsCount[t]; i++, c = (c + 1) % (sizeof(colors) / sizeof(colors[0])))
                    {
                        const auto total_begin = start_pos.y + ((char*)mAllocators[t][i]->MemoryBegin() - (char*)mAllocators[0][0]->MemoryBegin()) / byte_pr_pixel;
                        const auto total_end = total_begin + (mAllocators[t][i]->Capacity() / byte_pr_pixel);
                        const auto used_end = total_begin + (mAllocators[t][i]->AllocatedSize() / byte_pr_pixel);

                        list->AddRectFilled({x_begin, total_begin}, {x_end, total_end}, colors[c] | (0x77 << 24));
                        list->AddRectFilled({x_begin, total_begin}, {x_end, used_end}, colors[c] | (0xFF << 24));
                        list->AddRect({x_begin, total_begin}, {x_end, total_end}, 0xFF << 24);

                        ImGui::SetCursorScreenPos(ImVec2(x_begin, total_begin));
                        ImGui::Dummy({x_end - x_begin, total_end - total_begin});
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text("Allocator: %s\nCapacity: %d\nUsed: %d",
                                        mAllocators[t][i]->Name(),
                                        mAllocators[t][i]->Capacity(),
                                        mAllocators[t][i]->AllocatedSize());

                            ImGui::EndTooltip();
                        }
                    }
                }
            };

            ImGui::BeginChild(ImGui::GetID("Memory"), ImVec2(detailed_width, 0.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            {
                static int byte_pr_pixel = 64;
                ImGui::InputInt("##byte_pr_pixel", &byte_pr_pixel);
                byte_pr_pixel = std::max(byte_pr_pixel, 1);
                DrawMemory(byte_pr_pixel);
            }
            ImGui::EndChild();
            ImGui::SameLine();

            ImGui::BeginChild(ImGui::GetID("MemoryViewMinimap"), ImVec2(0.0f, 0.0f), true);
            {
                // For the dynamic minimap
                const auto& style = ImGui::GetStyle();
                const auto max_y = std::max(ImGui::GetContentRegionAvail().y - style.WindowPadding.y, 1.0f);
                const auto byte_pr_pixel = mAllocators[0][0]->Capacity() / max_y;
                DrawMemory(byte_pr_pixel);
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
