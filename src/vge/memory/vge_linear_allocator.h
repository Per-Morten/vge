#pragma once
#include <vge_allocator.h>
#include <vge_memory_manager.h>

namespace VGE
{
    class LinearAllocator
        : public VGE::Allocator
    {
    public:
        LinearAllocator(void* memory, int cap, const char* name = nullptr, MemoryManager* memory_manager = &VGE::gMemoryManager);
        virtual ~LinearAllocator();

        virtual void* Allocate(int size) VGE_NOEXCEPT override;
        virtual void Deallocate(void* ptr) VGE_NOEXCEPT override;
        virtual int AllocatedSize() const VGE_NOEXCEPT override;
        virtual void Clear() VGE_NOEXCEPT override;

        virtual int Capacity() const VGE_NOEXCEPT override;
        virtual const void* MemoryBegin() const VGE_NOEXCEPT override;
        virtual const char* Name() const VGE_NOEXCEPT override;

    private:
        char* mData;
        int mSize;
        int mCap;
        char mName[256];
        MemoryManager* mMemoryManager;

    };
}
