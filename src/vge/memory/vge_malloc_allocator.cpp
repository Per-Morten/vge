#include <vge_malloc_allocator.h>
#include <cstdlib>

void*
vge::malloc_allocator::allocate(int size) VGE_NOEXCEPT
{
    //VGE_DEBUG("%s: Allocating: %d", m_name, size);
    return std::malloc(size);
}

void
vge::malloc_allocator::deallocate(void* ptr) VGE_NOEXCEPT
{
    std::free(ptr);
}

int
vge::malloc_allocator::allocated_size() VGE_NOEXCEPT
{
    return 0;
}
