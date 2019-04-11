#include <catch.h>
#include <vge_memory.h>
#include <vge_thread.h>

TEST_CASE("Linear Allocator registers itself", "[linear_allocator]")
{
    char memory[256];
    VGE::MemoryManager manager;
    VGE::LinearAllocator allocator(memory, 256, nullptr, &manager);
    REQUIRE(manager.mAllocatorsCount[VGE::Thread::ThisThread::ID()] == 1);
}









