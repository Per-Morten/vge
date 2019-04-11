#include <cstring>
#include <algorithm>

template<class T>
VGE::Array<T>::Array(Allocator& allocator)
    : mAllocator(&allocator)
    , mSize(0)
    , mCap(2)
    , mData((T*)mAllocator->Allocate(sizeof(T) * mCap))
{
}

template<class T>
VGE::Array<T>::Array(Array&& other)
{
    mAllocator = other.mAllocator;
    mData = other.mData;
    mSize = other.mSize;
    mCap = other.mCap;

    other.mAllocator = nullptr;
    other.mData = nullptr;
    other.mSize = 0;
    other.mCap = 0;
}

template<class T>
VGE::Array<T>&
VGE::Array<T>::operator=(Array&& other)
{
    if (this != &other)
    {
        Clear();
        mAllocator->Deallocate(mData);
        mAllocator = other.mAllocator;
        mData = other.mData;
        mSize = other.mSize;
        mCap = other.mCap;

        other.mAllocator = nullptr;
        other.mData = nullptr;
        other.mSize = 0;
        other.mCap = 0;
    }

    return *this;
}

template<class T>
VGE::Array<T>::~Array()
{
    Clear();
    mAllocator->Deallocate(mData);
}

template<class T>
T&
VGE::Array<T>::Back()
{
    return mData[mSize - 1];
}

template<class T>
const T&
VGE::Array<T>::Back() const
{
    return mData[mSize - 1];
}

template<class T>
void
VGE::Array<T>::PushBack(const T& item)
{
    // If we need to grow
    if (mSize == mCap)
        Reserve(mSize * 2);

    new(&mData[mSize++])T(item);
}

template<class T>
void
VGE::Array<T>::Clear()
{
    if constexpr (!std::is_trivial_v<T>)
        for (int i = 0; i != mSize; i++)
            mData[i].~T();

    mSize = 0;
}

template<class T>
T&
VGE::Array<T>::operator[](int idx)
{
    VGE_ASSERT(idx >= 0 && idx < Size(), "idx: %d is not in valid range: [0, %d)", idx, Size());
    return mData[idx];
}

template<class T>
const T&
VGE::Array<T>::operator[](int idx) const
{
    VGE_ASSERT(idx >= 0 && idx < Size(), "idx: %d is not in valid range: [0, %d)", idx, Size());
    return mData[idx];
}

template<class T>
T*
VGE::Array<T>::Begin()
{
    return mData;
}

template<class T>
T*
VGE::Array<T>::End()
{
    return mData + mSize;
}

template<class T>
const T*
VGE::Array<T>::Begin() const
{
    return mData;
}

template<class T>
const T*
VGE::Array<T>::End() const
{
    return mData + mSize;
}

template<class T>
T*
VGE::Array<T>::Data()
{
    return mData;
}

template<class T>
const T*
VGE::Array<T>::Data() const
{
    return mData;
}

template<class T>
void
VGE::Array<T>::Resize(int new_size)
{
    if (new_size > mCap)
        Reserve(new_size);

    if (!std::is_trivial_v<T>)
        for (int i = mSize; i < new_size; i++)
            new(&mData[i])T();

    mSize = new_size;
}

template<class T>
int
VGE::Array<T>::Size() const
{
    return mSize;
}

template<class T>
int
VGE::Array<T>::Capacity() const
{
    return mCap;
}

template<class T>
void
VGE::Array<T>::Reserve(int new_cap)
{
    // we already have more capacity than the suggested new
    if (new_cap < mCap)
        return;

    auto new_begin = (T*)mAllocator->Allocate(new_cap * sizeof(T));

    if constexpr (std::is_trivially_copyable_v<T>)
    {
        std::memcpy(new_begin, mData, mSize * sizeof(T));
    }
    else
    {
        for (int i = 0; i != mSize; i++)
        {
            new(&new_begin[i])T(std::move(mData[i]));
            mData[i].~T();
        }
    }

    mAllocator->Deallocate(mData);
    mData = new_begin;
    mCap = new_cap;
}
