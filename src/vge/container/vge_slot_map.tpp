#include <vge_core.h>

template<class T>
VGE::SlotMap<T>::SlotMap(VGE::Allocator& allocator)
    : mAllocator(&allocator)
    , mHandles((Handle*)allocator.Allocate(2 * sizeof(Handle)))
    , mData((T*)allocator.Allocate(2 * sizeof(T)))
    , mErase((int*)allocator.Allocate(2 * sizeof(int)))
    , mSize(0)
    , mCap(2)
    , mFreeListHead(0)
    , mFreeListTail(1)
{
    mHandles[mFreeListHead].idx = mFreeListTail;
    mHandles[mFreeListHead].gen = 0;
    mHandles[mFreeListTail].idx = mFreeListTail;
    mHandles[mFreeListTail].gen = 0;
}

template<class T> typename
VGE::SlotMap<T>::Handle
VGE::SlotMap<T>::Insert(const T& item)
{
    if (mSize == mCap)
    {
        Reallocate(mCap * 2);
        // Need to re-assign mFreeListHead in this case, as it probably is the same as mFreeListTail at this point!
    }

    // Allocate
    const auto slot_idx = mFreeListHead;
    new(&mData[mSize])T(item);
    mErase[mSize] = slot_idx;

    // Pop off free-list
    const auto new_head = mHandles[mFreeListHead].idx;
    mFreeListHead = new_head;
    mHandles[slot_idx].idx = mSize++;

    return {slot_idx, mHandles[slot_idx].gen};
}

template<class T>
void
VGE::SlotMap<T>::Remove(const Handle& handle)
{
    VGE_ASSERT(mHandles[handle.idx].gen == handle.gen, "Trying to doubly free element: %d", handle.idx);

    mHandles[handle.idx].gen++;
    auto idx = mHandles[handle.idx].idx;


    // TODO: Fix this if T is trivial.
    mData[idx].~T();
    new(&mData[idx])T(std::move(mData[mSize - 1]));
    mErase[idx] = mErase[mSize - 1];

    // TODO: Update tail etc here.
    mSize--;
}

template<class T>
T*
VGE::SlotMap<T>::operator[](const Handle& handle)
{
    if (mHandles[handle.idx].gen != handle.gen)
        return nullptr;

    return &mData[mHandles[handle.idx].idx];
}

template<class T>
const T*
VGE::SlotMap<T>::operator[](const Handle& handle) const
{
    if (mHandles[handle.idx].gen != handle.gen)
        return nullptr;

    return &mData[mHandles[handle.idx].idx];
}

template<class T>
T&
VGE::SlotMap<T>::operator[](int idx)
{
    return mData[idx];
}

template<class T>
const T&
VGE::SlotMap<T>::operator[](int idx) const
{
    return mData[idx];
}

template<class T>
int
VGE::SlotMap<T>::Size() const
{
    return mSize;
}

template<class T>
int
VGE::SlotMap<T>::Capacity() const
{
    return mCap;
}

template<class T>
void
VGE::SlotMap<T>::Reallocate(int new_size)
{
    auto new_handles = (Handle*)mAllocator->Allocate(new_size * sizeof(Handle));
    auto new_data = (T*)mAllocator->Allocate(new_size * sizeof(T));
    auto new_erase = (int*)mAllocator->Allocate(new_size * sizeof(int));

    std::memcpy(new_handles, mHandles, mCap * sizeof(Handle));
    std::memcpy(new_erase, mErase, mCap * sizeof(int));

    // TODO: Deal with trivial classes
    std::memcpy(new_data, mData, mSize * sizeof(T));

    mAllocator->Deallocate(mHandles);
    mAllocator->Deallocate(mErase);

    // TODO: Deal with trivial classes
    mAllocator->Deallocate(mData);

    mHandles = new_handles;
    mErase = new_erase;
    mData = new_data;

    mCap = new_size;
}
