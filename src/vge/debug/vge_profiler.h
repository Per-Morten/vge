#pragma once
#include <vge_core.h>
#include <vge_allocator.h>
#include <vge_log.h>

#include <chrono>
#include <cstring>

namespace vge
{
    void initialize_profiler(vge::allocator& allocator) VGE_NOEXCEPT;
    void shutdown_profiler() VGE_NOEXCEPT;

    // lane_id: The id indicating which lane/thread to assign this event to.
    // duration: duration in ns.
    void push_profile_event(int lane_id, char* filename, char* function, int line, int duration) VGE_NOEXCEPT;

    void new_frame() VGE_NOEXCEPT;
    void draw_profiler() VGE_NOEXCEPT;

    namespace details
    {
        struct vge_raii_profile
        {
            using time_point = std::chrono::high_resolution_clock::time_point;

            time_point start;
            time_point stop;

            const char* filename;
            const char* function; // This does not necessarily work correctly, as __func__ is apparently not a macro, but rather a variable.
                                  // So, look if this becomes an issue in the profiler.
            int line;
            int lane_id = 0;

            vge_raii_profile(const char filename[], const char function[], int line)
            {
                #ifdef WIN32
                this->filename = std::strrchr(filename, '\\');
                #else
                this->filename = std::strrchr(filename, '/');
                #endif
                this->filename++;

                this->function = function;
                this->line = line;
                VGE_DEBUG("In VGE_RAII_PROFILE Constructor: %s, %s, %d", this->filename, function, line);
            }

            ~vge_raii_profile()
            {
                VGE_DEBUG("In VGE_RAII_PROFILE Destructor");
            }
        };
    }
}

#define VGE_MACRO_APPEND_(a, b) a ## b
#define VGE_MACRO_APPEND(a, b) VGE_MACRO_APPEND_(a, b)

#define VGE_PROFILE() auto VGE_MACRO_APPEND(profiler, __LINE__) = vge::details::vge_raii_profile(__FILE__, __func__, __LINE__)

