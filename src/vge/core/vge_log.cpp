#include <vge_log.h>
#include <imgui.h>
#include <atomic>

static std::atomic_flag print_protector;

void
vge_log(FILE* file,
       const char* type,
       const char* color,
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
    std::size_t size = 1 + vsnprintf(NULL, 0, fmt, args1);
    char buffer[size];

    va_end(args1);
    std::vsnprintf(buffer, size, fmt, args2);
    va_end(args2);

    #ifdef WIN32
    const char* filename = std::strrchr(filepath, '\\');
    #else
    const char* filename = std::strrchr(filepath, '/');
    #endif
    ++filename;

    #ifndef WIN32
    while (std::atomic_flag_test_and_set_explicit(&print_protector,
                                             std::memory_order_acquire))
    {

    }
    #endif

    std::fprintf(file, "%s[%-5s]%s: %-15s: %-25s:%4d: %s\n",
            color, type, VGE_COLOR_RESET,
            filename, func,
            line, buffer);

    std::fflush(file);

    #ifndef WIN32
    std::atomic_flag_clear_explicit(&print_protector,
                               std::memory_order_release);
    #endif
}

static constexpr int log_table_size = 1024;
static constexpr int log_message_size = 1024;
static char g_log_table[log_table_size][log_message_size];
static int g_log_idx = 0;

void
vge_log_imgui(const char* type,
              const char* color,
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
    std::size_t size = 1 + vsnprintf(NULL, 0, fmt, args1);
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

    g_log_idx = (g_log_idx + 1) % log_table_size;
    char* buff = g_log_table[g_log_idx];

    std::sprintf(buff, "[%-5s]: %-15s: %-25s:%4d: %s\n",
            type,
            filename, func,
            line, buffer);
}

void
vge_log_draw_imgui_debug()
{
    // Should separate this into different categories, so that it is easier to filter.
    if (ImGui::Begin("Log"))
    {
        // This will stop working as soon as we wrap around.
        for (int i = 0; i < log_table_size; i++)
        {
            ImGui::Text("%s", g_log_table[i]);
        }
    }
    ImGui::End();
}
