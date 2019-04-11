#include <vge_thread.h>
#include <vge_debug.h>

static thread_local VGE::Thread::ThreadID sThisID = 0;

// Should have some system in place to ensure that we never assign the same id to multiple threads, but,
// that shouldn't really happen, as I don't plan to expose the threading outside of a job system.

VGE::Thread::Thread(ThreadID id)
    : mID(id)
{
    VGE_ASSERT(id > 0, "Cannot assign id 0, as this is reserved for main thread");
}

VGE::Thread::ThreadID
VGE::Thread::ID() const
{
    return mID;
}

void
VGE::Thread::Start(std::function<void()> f)
{
    mThread = std::thread([this,f=std::move(f)]()
                          {
                                sThisID = this->mID;
                                f();
                          });
}

void
VGE::Thread::Join()
{
    // Need to add assert here, that we are not trying to join our own thread
    VGE_ASSERT(VGE::Thread::ThisThread::ID() != mID, "Trying to join your own thread!");
    mThread.join();
}

VGE::Thread::ThreadID
VGE::Thread::ThisThread::ID()
{
    return sThisID;
}
