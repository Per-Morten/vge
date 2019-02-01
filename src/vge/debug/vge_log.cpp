#include <vge_log.h>
#include <vge_assert.h>
#include <imgui.h>
#include <algorithm>

#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#include <glad/glad.h>
#include <vge_gfx_gl.h>

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
vge::init_logger()
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

/////////////////////////////////////////////////////////////////////
/// GL Logging
/////////////////////////////////////////////////////////////////////
struct gl_debug_msg
{
    GLenum source;
    GLenum type;
    GLuint id;
    GLenum severity;
    char message[log_message_size];
    // Should we also store backtrace?
};

gl_debug_msg g_gl_table[log_table_size];
int g_gl_idx;

static void APIENTRY
gl_debug_callback(GLenum source,
                  GLenum type,
                  GLuint id,
                  GLenum severity,
                  GLsizei length,
                  const GLchar *message,
                  const void *userParam)
{
    VGE_ASSERT((size_t)length < sizeof(gl_debug_msg::message), "Message: %s, does not fit in buffer", message);

    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    auto buff = &g_gl_table[g_gl_idx];
    g_gl_idx = (g_gl_idx + 1) % log_table_size;
    buff->id = id;
    buff->type = type;
    buff->severity = severity;
    buff->source = source;
    std::strcpy(buff->message, message);
}

void
vge::init_gl_logger()
{
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

/////////////////////////////////////////////////////////////////////
/// Visualization
/////////////////////////////////////////////////////////////////////
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
    if (ImGui::BeginTabBar("LoggingTab"))
    {
        if (ImGui::BeginTabItem("CPU"))
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
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Graphics"))
        {
            static bool bound_to_bottom = true;
            ImGui::Checkbox("Bind to bottom", &bound_to_bottom); ImGui::SameLine();
            bool scroll_to_top = ImGui::SmallButton("Scroll to top");
            bound_to_bottom = (bound_to_bottom && !scroll_to_top);

            ImGui::BeginChild(ImGui::GetID("extended_logging"), ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

            int begin = g_gl_idx;

            for (int i = 0; i < log_table_size; i++, begin = (begin + 1) % log_table_size)
            {
                auto msg = &g_gl_table[begin];
                if (msg->id == 0)
                    continue;

                if (msg->type == GL_DEBUG_TYPE_ERROR || msg->type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
                 || msg->type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR || msg->type == GL_DEBUG_TYPE_PORTABILITY
                 || msg->severity == GL_DEBUG_SEVERITY_HIGH)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.800f, 0.160f, 0.160f, 1.0f));
                else if (msg->type == GL_DEBUG_TYPE_PERFORMANCE || msg->severity == GL_DEBUG_SEVERITY_MEDIUM)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.800f, 0.500f, 0.160f, 1.0f));
                else if (msg->severity == GL_DEBUG_SEVERITY_LOW)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                else
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.000f, 0.700f, 1.000f, 1.0f));

                ImGui::TextWrapped("[%s]: %s %s: %s",
                            gfx::gl_enum_to_string(msg->source),
                            gfx::gl_enum_to_string(msg->type),
                            gfx::gl_enum_to_string(msg->severity),
                            msg->message);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("msg id: %d", msg->id);

                ImGui::PopStyleColor();
            }

            if (bound_to_bottom) ImGui::SetScrollY(ImGui::GetScrollMaxY());
            else if (scroll_to_top) ImGui::SetScrollY(0.0f);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();
}
