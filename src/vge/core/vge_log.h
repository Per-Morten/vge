#pragma once
#include <vge_attributes.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#ifdef WIN32
#define VGE_COLOR_RESET ""
#define VGE_COLOR_FG_BLACK ""
#define VGE_COLOR_FG_RED ""
#define VGE_COLOR_FG_GREEN ""
#define VGE_COLOR_FG_YELLOW ""
#define VGE_COLOR_FG_BLUE ""
#define VGE_COLOR_FG_MAGENTA ""
#define VGE_COLOR_FG_CYAN ""
#define VGE_COLOR_FG_GREY ""
#define VGE_COLOR_FG_WHITE ""

#define VGE_COLOR_BG_BLACK ""
#define VGE_COLOR_BG_RED ""
#define VGE_COLOR_BG_GREEN ""
#define VGE_COLOR_BG_YELLOW ""
#define VGE_COLOR_BG_BLUE ""
#define VGE_COLOR_BG_MAGENTA ""
#define VGE_COLOR_BG_CYAN ""
#define VGE_COLOR_BG_GREY ""
#define VGE_COLOR_BG_WHITE ""

#else
#define VGE_COLOR_RESET "\033[0m"
#define VGE_COLOR_FG_BLACK "\033[0;30m"
#define VGE_COLOR_FG_RED "\033[0;31m"
#define VGE_COLOR_FG_GREEN "\033[0;32m"
#define VGE_COLOR_FG_YELLOW "\033[0;33m"
#define VGE_COLOR_FG_BLUE "\033[0;34m"
#define VGE_COLOR_FG_MAGENTA "\033[0;35m"
#define VGE_COLOR_FG_CYAN "\033[0;36m"
#define VGE_COLOR_FG_GREY "\033[0;37m"
#define VGE_COLOR_FG_WHITE "\033[0m"

#define VGE_COLOR_BG_BLACK "\033[0;40m"
#define VGE_COLOR_BG_RED "\033[0;41m"
#define VGE_COLOR_BG_GREEN "\033[0;42m"
#define VGE_COLOR_BG_YELLOW "\033[0;43m"
#define VGE_COLOR_BG_BLUE "\033[0;44m"
#define VGE_COLOR_BG_MAGENTA "\033[0;45m"
#define VGE_COLOR_BG_CYAN "\033[0;46m"
#define VGE_COLOR_BG_GREY "\033[0;47m"
#define VGE_COLOR_BG_WHITE "\033[0m"
#endif



///////////////////////////////////////////////////////////
/// \brief
///     Actual function which is called by log
///     macros.
///
/// \note
///     Do not use this function!
///     You are supposed to use the logger macros.
///
/// \todo
///     Add timing functionality.
///
/// \todo
///     Make proper buffered logger which does not
///     require a lock.
///     But rather logs to a specified location,
///     and is later printed out by the debug thread.
///////////////////////////////////////////////////////////
void
vge_log(FILE* file,
       const char* type,
       const char* color,
       const char* filepath,
       const char* func,
       const int line,
       const char* fmt,
       ...);

void
vge_log_imgui(const char* type,
              const char* color,
              const char* filepath,
              const char* func,
              const int line,
              const char* fmt,
              ...);

///////////////////////////////////////////////////////////
/// \ingroup vge_core
///
/// \brief
///     Prints the specified error to stderr and
///     terminates the program.
///
/// \detailed
///     This error is supposed to be used for
///     unrecoverable errors.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#ifdef WIN32
#define TERMINAL_PAUSE system("pause");
#else
#define TERMINAL_PAUSE
#endif

#define VGE_ERROR(fmt, ...)                                                                      \
{                                                                                               \
    vge_log(stderr, "ERROR", VGE_COLOR_FG_RED, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
    TERMINAL_PAUSE;                                                                             \
    exit(EXIT_FAILURE);                                                                         \
}

///////////////////////////////////////////////////////////
/// \ingroup vge_core
///
/// \brief
///     Prints the specified warning to stderr.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#define VGE_WARN(fmt, ...) \
vge_log(stderr, "WARN", VGE_COLOR_FG_YELLOW, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \ingroup vge_core
///
/// \brief
///     Prints debug information to stdout.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#define VGE_DEBUG(fmt, ...) \
vge_log(stdout, "DEBUG", VGE_COLOR_FG_CYAN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
vge_log_imgui("DEBUG", VGE_COLOR_FG_CYAN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \

///////////////////////////////////////////////////////////
/// \ingroup vge_core
///
/// \brief
///     Prints regular info to stdout.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#define VGE_INFO(fmt, ...) \
vge_log(stdout, "INFO", VGE_COLOR_FG_WHITE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);


void
vge_log_draw_imgui_debug();
