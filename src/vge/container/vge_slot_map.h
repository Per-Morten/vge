#pragma once
#include <vge_assert.h>
#include <vge_allocator.h>

// Implementation inspired by: https://www.youtube.com/watch?v=SHaAR7XPtNU
namespace VGE
{
    // Want tests for this thing!, particularly to test that it actually only allocates new elements when it needs to
    template<class T>
    class SlotMap
    {
    public:
        struct Handle
        {
            int idx;
            int gen;
        };

        SlotMap(VGE::Allocator& allocator = *(GetDefaultAllocator()));

        Handle
        Insert(const T& item);

        void
        Remove(const Handle& handle);

        T*
        operator[](const Handle& handle);

        const T*
        operator[](const Handle& handle) const;

        T&
        operator[](int idx);

        const T&
        operator[](int idx) const;

        int
        Size() const;

        int
        Capacity() const;

        T* Data();

    private:
        void Reallocate(int new_size);

        // Move over to using raw memory here.
        VGE::Allocator* mAllocator;
        Handle* mHandles;
        T* mData;
        int* mErase; // TODO: Find better name, an object located at the same place as a piece of data in mData here, holds the index to the corresponding handle in mHandles
        int mSize;
        int mCap;

        //VGE::Array<Handle> mHandles;
        //VGE::Array<T> mData;
        //VGE::Array<int> mErase; // To map from mData back to mHandles

        int mFreeListHead;
        int mFreeListTail;

        // Add FreeList
        // The handle effectivly works 2 ways,
        // the index in the handle we get from the user is used to index into the mHandles array
        // the index existing within the handles within mHandles table is a handle into the mData array



    };
}

#include <vge_slot_map.tpp>

