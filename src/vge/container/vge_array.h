#pragma once
#include <type_traits>
#include <vge_allocator.h>

namespace VGE
{
    // Dynamically resizable array.
    template<class T>
    class Array
    {
    public:
        Array(Allocator& allocator = *GetDefaultAllocator());
        Array(Array&&);
        Array& operator=(Array&&);
        Array(const Array& other) = delete;
        Array& operator=(const Array& other) = delete;
        ~Array();

        T& Back();
        const T& Back() const;

        void PushBack(const T& item);
        void Clear();

        T& operator[](int idx);
        const T& operator[](int idx) const;

        T* Begin();
        T* End();
        const T* Begin() const;
        const T* End() const;

        T* Data();
        const T* Data() const;

        void Resize(int new_size);
        int Size() const;

        void Reserve(int new_cap);
        int Capacity() const;

    private:
        Allocator* mAllocator{};
        int mSize;
        int mCap;
        T* mData{};
    };
}

#include <vge_array.tpp>

