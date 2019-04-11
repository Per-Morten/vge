#include <vge_linear_allocator.h>
#include <vge_core.h>
#include <vge_assert.h>
#include <cstring>

VGE::LinearAllocator::LinearAllocator(void* memory, int cap, const char* name, MemoryManager* manager)
    : mData((char*)memory)
    , mSize(0)
    , mCap(cap)
    , mMemoryManager(manager)
{
    auto tmp = (!name) ? "unnamed" : name;
    std::strcpy(mName, tmp);
    mMemoryManager->RegisterAllocator(this);
}

VGE::LinearAllocator::~LinearAllocator()
{
    mMemoryManager->DeregisterAllocator(this);
}

void*
VGE::LinearAllocator::Allocate(int size) VGE_NOEXCEPT
{
    VGE_ASSERT(mSize + size <= mCap, "Overrunning linear allocator: %s", mName);
    int old = mSize;
    mSize += size;
    return &mData[old];
}

void
VGE::LinearAllocator::Deallocate(VGE_UNUSED void* ptr) VGE_NOEXCEPT
{

}

void
VGE::LinearAllocator::Clear() VGE_NOEXCEPT
{
    mSize = 0;
}

int
VGE::LinearAllocator::Capacity() const VGE_NOEXCEPT
{
    return mCap;
}

int
VGE::LinearAllocator::AllocatedSize() const VGE_NOEXCEPT
{
    return mSize;
}

const void*
VGE::LinearAllocator::MemoryBegin() const VGE_NOEXCEPT
{
    return mData;
}

const char*
VGE::LinearAllocator::Name() const VGE_NOEXCEPT
{
    return mName;
}

