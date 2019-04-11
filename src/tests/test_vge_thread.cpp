#include <catch.h>
#include <vge_thread.h>
#include <atomic>

TEST_CASE("Main thread gets threadID 0", "[thread]")
{
    REQUIRE(VGE::Thread::ThisThread::ID() == 0);
}

TEST_CASE("New threads gets supplied ID", "[thread]")
{
    VGE::Thread thread(2);
    REQUIRE(thread.ID() == 2);
    REQUIRE(VGE::Thread::ThisThread::ID() == 0);

    std::atomic<bool> success = false;
    thread.Start([&success]()
    {
        success.store(VGE::Thread::ThisThread::ID() == 2);
    });

    thread.Join();
    REQUIRE(success.load() == true);
}
