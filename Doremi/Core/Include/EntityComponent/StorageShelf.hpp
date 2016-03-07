#pragma once
// Project specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <type_traits>
#include <Doremi/Core/Include/EntityComponent/Components/LowerSkeletalAnimationComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/SkeletalAnimationComponent.hpp>

// where we store the actual components/data
template <typename T> class StorageShelf
{
public:
    static StorageShelf<T>* GetInstance();
    T* mItems;

    T* GetPointerToArray() { return mItems; }

private:
    StorageShelf() : mItems(nullptr) { mItems = new T[MAX_NUM_ENTITIES](); }

    StorageShelf(const StorageShelf& t_item) = delete;

    ~StorageShelf() { FreeHelper(); }

    void FreeHelper() { delete[] mItems; }
};

// Temporary fix, when FreeHelper is called and the Template is LowerSkeletalAnimationComponent, do nothing, as the usual destruction crashes, TODOXX,
// TODORT
void StorageShelf<Doremi::Core::LowerSkeletalAnimationComponent>::FreeHelper() {}

// Temporary fix, when FreeHelper is called and the Template is SkeletalAnimationComponent, do nothing, as the usual destruction crashes, TODOXX,
// TODORT
void StorageShelf<Doremi::Core::SkeletalAnimationComponent>::FreeHelper() {}

template <class T> StorageShelf<T>* StorageShelf<T>::GetInstance()
{
    static StorageShelf<T> storageShelf;
    return &storageShelf;
}

// This is the magic about singletons
// Find the right shelf for the needed component
template <class T> static T* GetComponent(EntityID pEntityID)
{
    StorageShelf<T>* tNeededShelf = StorageShelf<T>::GetInstance();

    return &tNeededShelf->mItems[pEntityID];
}

/**
    Fist paremeter is from, second parameter is to
*/
template <class T, class U> static void CloneShelf()
{
    if(sizeof(T) != sizeof(U))
    {
        std::runtime_error("Attempting to memcpy two different sized shelfs!");
    }

    // Get Pointers
    StorageShelf<T>* tFirstShelf = StorageShelf<T>::GetInstance();
    StorageShelf<U>* tSecondShelf = StorageShelf<U>::GetInstance();

    // Memcpy
    // TODO could take parameter of how many entities we have active
    memcpy(tSecondShelf->mItems, tFirstShelf->mItems, sizeof(T) * MAX_NUM_ENTITIES);
}
