#include <vge_allocator.h>
#include <vge_malloc_allocator.h>
#include <vge_debug.h>
#include <vge_global_allocator.h>
#include <cstring>

// VGE::Allocator::Allocator(const char* name) VGE_NOEXCEPT
// {
//     VGE_ASSERT(strlen(name) < max_name_length, "Allocator name: %s is to long (%d of %d characters)", name, std::strlen(name), max_name_length);
//     std::strcpy(mName, name);
// }

// const char*
// VGE::Allocator::Name() const VGE_NOEXCEPT
// {
//     return mName;
// }

VGE::Allocator*
VGE::GetDefaultAllocator()
{
    static MallocAllocator a("default_allocator");
    return &a;
}

