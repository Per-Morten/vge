#include <vge_allocator.h>
#include <cstring>

vge::allocator::allocator(const char* name) VGE_NOEXCEPT
{
    VGE_ASSERT(strlen(name) < max_name_length, "Name To Long");
    std::strcpy(m_name, name);
}

