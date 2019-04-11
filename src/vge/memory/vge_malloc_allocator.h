#pragma once
#include <vge_allocator.h>

namespace VGE
{
    class //[[deprecated("From now on all memory allocation should happen through an allocator")]]
    MallocAllocator
        : public VGE::Allocator
    {
    public:
        MallocAllocator(const char* name [[maybe_unused]] ) {}
        virtual ~MallocAllocator() {}

        virtual void* Allocate(int size) VGE_NOEXCEPT override;
        virtual void Deallocate(void* ptr) VGE_NOEXCEPT override;
        virtual int AllocatedSize() const VGE_NOEXCEPT override;
        virtual void Clear() VGE_NOEXCEPT {}
    };
}

