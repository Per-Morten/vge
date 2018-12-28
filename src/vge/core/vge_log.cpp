#include <vge_log.h>
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
    const char* filename = strrchr(filepath, '\\');
    #else
    const char* filename = strrchr(filepath, '/');
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