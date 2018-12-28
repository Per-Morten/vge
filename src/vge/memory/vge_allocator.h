#pragma once
#include <vge_core.h>

namespace vge
{
    class allocator
    {
    public:
        allocator(const char* name) VGE_NOEXCEPT;

        virtual void* allocate(int size) VGE_NOEXCEPT = 0;
        virtual void deallocate(void* ptr) VGE_NOEXCEPT = 0;
        virtual int allocated_size() VGE_NOEXCEPT = 0;

    protected:
        static constexpr int max_name_length = 256;
        char m_name[max_name_length];
    };
}
