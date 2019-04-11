#pragma once
#include <vge_allocator.h>
#include <vge_memory_manager.h>
// Note, do not use this! Only putting this in header to allow for testing.

namespace VGE
{
    class GlobalAllocator
        : public VGE::Allocator
    {
    public:
        GlobalAllocator(MemoryManager* memory_manager = &gMemoryManager);
        virtual ~GlobalAllocator();

        virtual void* Allocate(int size) VGE_NOEXCEPT override;
        virtual void Deallocate(void* ptr) VGE_NOEXCEPT override;
        virtual int AllocatedSize() const VGE_NOEXCEPT override;
        virtual void Clear() VGE_NOEXCEPT override {}

        virtual int Capacity() const VGE_NOEXCEPT override;
        virtual const void* MemoryBegin() const VGE_NOEXCEPT override;
        virtual const char* Name() const VGE_NOEXCEPT override;

    private:
        char* mData{};
        int mCap{};
        int mSize{}; // For testing
        MemoryManager* mMemoryManager{};
    };

    inline GlobalAllocator gGlobalAllocator;
}
