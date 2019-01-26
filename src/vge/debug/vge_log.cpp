#include <vge_log.h>
#include <imgui.h>
#include <algorithm>

#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

static constexpr int log_table_size = 1024;
static constexpr int log_message_size = 1024;

static char g_log_table[log_table_size][log_message_size];
static int g_log_idx = 0;

void
vge_log(const char* type,
        const char* filepath,
        const char* func,
        const int line,
        const char* fmt,
        ...)
{
    std::va_list args1;
    va_start(args1, fmt);
    std::va_list args2;
    va_copy(args2, args1);
    std::size_t size = 1 + std::vsnprintf(nullptr, 0, fmt, args1);
    char buffer[size];

    va_end(args1);
    std::vsnprintf(buffer, size, fmt, args2);
    va_end(args2);

    // Need to add an assert that there is actually room!
    #ifdef WIN32
    const char* filename = std::strrchr(filepath, '\\');
    #else
    const char* filename = std::strrchr(filepath, '/');
    #endif
    ++filename;

    char* buff = g_log_table[g_log_idx];
    g_log_idx = (g_log_idx + 1) % log_table_size;

    std::sprintf(buff, "[%-5s]: %-15s: %-25s:%4d: %s\n",
                 type,
                 filename, func,
                 line, buffer);
}


void
vge::initialize_logger()
{
    auto handler = [](int sig, siginfo_t* si, void* unused)
    {
        // Check to see if we have wrapped around once
        // No need to do strlen, just check that the first spot isn't \0!
        int begin = std::strlen(g_log_table[(g_log_idx + 1) % log_table_size])
                  ? (g_log_idx + 1) % log_table_size
                  : 0;

        while (begin != g_log_idx)
        {
            std::fprintf(stderr, "%s", g_log_table[begin]);
            begin = (begin + 1) % log_table_size;
        }

        void* array[25];
        size_t size = backtrace(array, 25);

        // print out all the frames to stderr
        fprintf(stderr, "FATAL ERROR: signal %d:\n", sig);
        backtrace_symbols_fd(array, size, STDERR_FILENO);

        exit(EXIT_FAILURE);
    };

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGSEGV, &sa, nullptr) == -1)
        VGE_ERROR("Couldn't set up SIGSEGV handler");

    if (sigaction(SIGILL, &sa, nullptr) == -1)
        VGE_ERROR("Couldn't set up SIGILL handler");
}

void
vge::draw_log_overlay(int lines_count)
{
    // Should separate log into different categories, so that it is easier to filter.
    if (ImGui::BeginChild(ImGui::GetID("log_overlay"), ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoScrollbar))
    {
        int end = g_log_idx;
        int begin = end - lines_count;
        begin += log_table_size * (begin < 0);

        for (; begin != end; begin = (begin + 1) % log_table_size)
        {
            if (g_log_table[begin][0] == '\0')
                continue;

            if (!std::strncmp(g_log_table[begin], "[DEBUG", sizeof("[DEBUG") - 1)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.000f, 0.700f, 1.000f, 1.0f));
            else if (!std::strncmp(g_log_table[begin], "[INFO", sizeof("[INFO") - 1))   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            else if (!std::strncmp(g_log_table[begin], "[WARN", sizeof("[WARN") - 1))   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.800f, 0.500f, 0.160f, 1.0f));
            else if (!std::strncmp(g_log_table[begin], "[ERROR", sizeof("[ERROR") - 1)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.800f, 0.160f, 0.160f, 1.0f));
            else VGE_ERROR("Message: %s, is not correct format", g_log_table[begin]);

            ImGui::TextWrapped("%s", g_log_table[begin]);
            ImGui::PopStyleColor();
        }
    }

    ImGui::EndChild();
}

void
vge::draw_log_window()
{
    static bool bound_to_bottom = true;
    ImGui::Checkbox("Bind to bottom", &bound_to_bottom); ImGui::SameLine();
    bool scroll_to_top = ImGui::SmallButton("Scroll to top");
    bound_to_bottom = (bound_to_bottom && !scroll_to_top);

    ImGui::BeginChild(ImGui::GetID("extended_logging"), ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    int begin = g_log_idx;

    for (int i = 0; i < log_table_size; i++, begin = (begin + 1) % log_table_size)
    {
        if (g_log_table[begin][0] == '\0')
            continue;

        if (!std::strncmp(g_log_table[begin], "[DEBUG", sizeof("[DEBUG") - 1)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.000f, 0.700f, 1.000f, 1.0f));
        else if (!std::strncmp(g_log_table[begin], "[INFO", sizeof("[INFO") - 1))   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        else if (!std::strncmp(g_log_table[begin], "[WARN", sizeof("[WARN") - 1))   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.800f, 0.500f, 0.160f, 1.0f));
        else if (!std::strncmp(g_log_table[begin], "[ERROR", sizeof("[ERROR") - 1)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.800f, 0.160f, 0.160f, 1.0f));
        else VGE_ERROR("Message: %s, is not correct format", g_log_table[begin]);

        ImGui::TextUnformatted(g_log_table[begin]);
        ImGui::PopStyleColor();
    }

    if (bound_to_bottom) ImGui::SetScrollY(ImGui::GetScrollMaxY());
    else if (scroll_to_top) ImGui::SetScrollY(0.0f);


    ImGui::EndChild();
}
