#pragma once
#include <vge_allocator.h>

namespace vge
{
    class malloc_allocator
        : public vge::allocator
    {
    public:
        using allocator::allocator;

        virtual void* allocate(int size) VGE_NOEXCEPT override;
        virtual void deallocate(void* ptr) VGE_NOEXCEPT override;
        virtual int allocated_size() VGE_NOEXCEPT override;
    };

    inline malloc_allocator malloc("global_allocator");
}

