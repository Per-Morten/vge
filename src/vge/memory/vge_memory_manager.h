#pragma once
#include <vge_allocator.h>
#include <vge_thread.h>

namespace VGE
{
    struct MemoryManager
    {
        void RegisterAllocator(Allocator* allocator);
        void DeregisterAllocator(Allocator* allocator);

        void DrawDebug();

        // Currently, just limitting it to 256 allocators.
        static constexpr auto MaxAllocators = 256;

        Allocator* mAllocators[VGE::Thread::MaxThreads][MaxAllocators]{};
        int mAllocatorsCount[VGE::Thread::MaxThreads]{};
    };

    inline MemoryManager gMemoryManager;
}

