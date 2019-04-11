#pragma once
#include <vge_attributes.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <vge_thread.h>

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
///     and is later printed out by a debug thread.
///////////////////////////////////////////////////////////
void
vge_log(const char* type,
        const char* filepath,
        const char* func,
        const int line,
        VGE::Thread::ThreadID thread_id,
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
#define VGE_ERROR(fmt, ...)                                                                             \
{                                                                                                       \
    vge_log("ERROR", __FILE__, __func__, __LINE__, VGE::Thread::ThisThread::ID(), fmt, ##__VA_ARGS__);  \
    raise(SIGTRAP);                                                                                     \
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
vge_log("WARN", __FILE__, __func__, __LINE__, VGE::Thread::ThisThread::ID(), fmt, ##__VA_ARGS__);

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
vge_log("DEBUG", __FILE__, __func__, __LINE__, VGE::Thread::ThisThread::ID(), fmt, ##__VA_ARGS__); \


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
vge_log("INFO", __FILE__, __func__, __LINE__, VGE::Thread::ThisThread::ID(), fmt, ##__VA_ARGS__);

namespace vge
{
    void init_logger();
    void init_gl_logger();

    void draw_log_overlay(int lines_count);

    void draw_log_window();
}
