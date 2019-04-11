#pragma once
#include <vge_core.h>
#include <functional>
#include <thread>

namespace VGE
{
    class Thread
    {
    public:
        using ThreadID = int;
        static constexpr auto MaxThreads = 8; // Including main thread, meaning that you can create at most 7 instances of VGE::Thread.

        struct ThisThread
        {
            static VGE::Thread::ThreadID ID();
        };

        Thread(ThreadID id);

        ThreadID ID() const;

        // TODO: Switch this to being a pure function ptr with void* or something?
        void Start(std::function<void()> f);
        void Join();

    private:
        Thread() {}; // Needed just so the main thread gets assigned an ID of 0
        ThreadID mID{};
        std::thread mThread{};
    };
}
