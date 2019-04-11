// Note, do not use this! Only putting this in header to allow for testing.
#include <vge_global_allocator.h>
#include <vge_memory.h>

VGE::GlobalAllocator::GlobalAllocator(VGE::MemoryManager* memory_manager)
    : mMemoryManager(memory_manager)
{
    mSize = 0;
    // Just allocate a gig, should be enough for now.

    //mCap = 1 << 30;
    mCap = 1 << 20;
    //mCap = 1 << 12;
    mData = (char*)malloc(mCap);

    mMemoryManager->RegisterAllocator(this);
}

VGE::GlobalAllocator::~GlobalAllocator()
{
    mMemoryManager->DeregisterAllocator(this);
    free(mData);
}

void*
VGE::GlobalAllocator::Allocate(int size) VGE_NOEXCEPT
{
    int old = mSize;
    mSize += size;
    return &mData[old];
}

void
VGE::GlobalAllocator::Deallocate(VGE_UNUSED void* ptr) VGE_NOEXCEPT
{

}

int
VGE::GlobalAllocator::AllocatedSize() const VGE_NOEXCEPT
{
    return 0; // For now at least, because we aren't really interested in what the global allocator has allocated
}

int
VGE::GlobalAllocator::Capacity() const VGE_NOEXCEPT
{
    return mCap;
}

const void*
VGE::GlobalAllocator::MemoryBegin() const VGE_NOEXCEPT
{
    return mData;
}

const char*
VGE::GlobalAllocator::Name() const VGE_NOEXCEPT
{
    return "global_allocator";
}
