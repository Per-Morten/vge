#include <vge_malloc_allocator.h>
#include <cstdlib>

void*
VGE::MallocAllocator::Allocate(int size) VGE_NOEXCEPT
{
    //VGE_DEBUG("%s: Allocating: %d", m_name, size);
    return std::malloc(size);
}

void
VGE::MallocAllocator::Deallocate(void* ptr) VGE_NOEXCEPT
{
    std::free(ptr);
}

int
VGE::MallocAllocator::AllocatedSize() const VGE_NOEXCEPT
{
    return 0;
}
