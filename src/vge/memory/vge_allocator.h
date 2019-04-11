#pragma once
#include <vge_core.h>

// TODO: Should have an own Resource/Static Allocator, for stuff that is static.
// TODO: Move this to be purely an interface, no need to store name in here!
namespace VGE
{
    class Allocator
    {
    public:
        virtual ~Allocator() VGE_NOEXCEPT {};

        virtual void* Allocate(int size) VGE_NOEXCEPT = 0;
        virtual void Deallocate(void* ptr) VGE_NOEXCEPT = 0;
        virtual int AllocatedSize() const VGE_NOEXCEPT = 0;
        virtual void Clear() VGE_NOEXCEPT = 0; // Deallocate all memory

        // Metadata for visualization - Only implemented to avoid compiler errors with malloc allocator
        virtual int Capacity() const VGE_NOEXCEPT {return 0;}
        virtual const void* MemoryBegin() const VGE_NOEXCEPT {return nullptr;}
        virtual const char* Name() const VGE_NOEXCEPT {return "unnamed";}
    };

    Allocator*
    GetDefaultAllocator();
}
