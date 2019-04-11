#pragma once
#include <vge_log.h>
#include <cstdlib>

///////////////////////////////////////////////////////////
/// \ingroup vge_core
///
/// \brief
///     Simple custom assert message that support
///     different functionality depending on build
///     settings.
///
/// \detailed
///     The VGE_ASSERT has four different settings
///     based on build defines.
///
/// \par VGE_ASSERT_TERMINATE
///     With this setting each failed assertion
///     will lead to a termination of the program.
///
/// \par VGE_ASSERT_REPORT
///     With this setting each failed assertion
///     will be a warning that is printed to the window.
///
/// \par VGE_ASSERT_THROW
///     With this setting each failed assertion
///     will result in raising a global error.
///     This behavior should not be used to recover
///     from as it is mainly used to avoid termination
///     when testing.
///
/// \par No definitions
///     In the case where there are no definitions
///     the assert will be empty, and the checks
///     are optimized out.
///////////////////////////////////////////////////////////
#if defined(VGE_ASSERT_TERMINATE)
#define VGE_NOEXCEPT noexcept
#define VGE_ASSERT(expr, fmt, ...)                                  \
if (VGE_LIKELY(expr)) { }                                           \
else                                                                \
{                                                                   \
    VGE_ERROR("Assertion failure: " #expr ": " fmt, ##__VA_ARGS__); \
}

#elif defined(VGE_ASSERT_REPORT)
#define VGE_NOEXCEPT noexcept
#define VGE_ASSERT(expr, fmt, ...)                                  \
if (VGE_LIKELY(expr)) { }                                           \
else                                                                \
{                                                                   \
    VGE_WARN("Assertion failure: " #expr ": " fmt, ##__VA_ARGS__);  \
}

#elif defined(VGE_ASSERT_THROW)
#include <stdexcept>
#define VGE_NOEXCEPT
#define VGE_ASSERT(expr, fmt, ...)                                  \
if (VGE_LIKELY(expr)) { }                                           \
else                                                                \
{                                                                   \
    char buffer[1024];                                              \
    std::sprintf(buffer, "Assertion failure: " #expr ": " fmt, ##__VA_ARGS__); \
    throw std::runtime_error(buffer);                                  \
}

#else
#define VGE_NOEXCEPT noexcept
#define VGE_ASSERT(expr, fmt, ...)
#endif

// Static assert does not get defined when compiling for windows
#ifdef WIN32
#define static_assert _Static_assert
#endif
