#pragma once
#include <vge_core.h>
#include <vge_allocator.h>
#include <vge_log.h>

#include <chrono>
#include <cstring>

// TODO: Create a clock class.
// TODO: Set a max for how many Profiling events we should keep.

namespace VGE
{
    using ProfileTimePoint = std::chrono::high_resolution_clock::time_point;

    struct ProfileEvent
    {
        ProfileTimePoint Begin{};
        ProfileTimePoint End{};
        const char* Label{};
        const char* Function{};
        int Line{};

        ProfileEvent() = default;
        ProfileEvent(const char* label,
                     const char* function,
                     int line,
                     ProfileTimePoint begin,
                     ProfileTimePoint end)
            : Begin(begin)
            , End(end)
            , Label(label)
            , Function(function)
            , Line(line)
            {}
    };

    // TODO: Want a way to log profiling data to disk, so I can use the profiler in chrome.
    struct Profiler
    {
        // Think of having init and shutdown functions
        void PushProfileEvent(const ProfileEvent& event);

        void BeginFrame();
        void EndFrame();
        void DrawProfiler();

        // Members
        // Temporary, find a better solution, or set off space for more.
        constexpr static auto MaxEvents = 256;
        constexpr static auto MaxFrames = 90;

        // These should be aligned based on cacheline, to avoid false sharing.
        ProfileEvent mEvents[VGE::Thread::MaxThreads][MaxEvents] = {};
        int mEventsCount[VGE::Thread::MaxThreads]{};

        ProfileTimePoint mFrameStart{};
    };

    inline Profiler gProfiler;

    struct RAIIProfiler
    {
        RAIIProfiler(const char* label,
                     const char* function,
                     int line,
                     Profiler* profiler = &gProfiler)
            : Function(function)
            , Line(line)
            , Begin(std::chrono::high_resolution_clock::now())
            , Receiver(profiler)
        {
            #ifdef WIN32
            Label = std::strrchr(label, '\\');
            #else
            Label = std::strrchr(label, '/');
            #endif
            Label = (Label) ? Label + 1 : label;
        }

        ~RAIIProfiler()
        {
            ProfileEvent e(Label, Function, Line, Begin, std::chrono::high_resolution_clock::now());
            Receiver->PushProfileEvent(e);
        }

        ProfileTimePoint Begin;
        const char* Label;
        const char* Function;
        Profiler* Receiver;
        int Line;
    };

    #define VGE_MACRO_APPEND_(a, b) a ## b
    #define VGE_MACRO_APPEND(a, b) VGE_MACRO_APPEND_(a, b)

    #define VGE_PROFILE() auto VGE_MACRO_APPEND(profiler, __LINE__) = VGE::RAIIProfiler(__FILE__, __func__, __LINE__)
    #define VGE_PROFILE_LABEL(x) auto VGE_MACRO_APPEND(profiler, __LINE__) = VGE::RAIIProfiler((x), __func__, __LINE__)

}


