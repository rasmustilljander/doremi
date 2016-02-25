#pragma once
// Project specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

using namespace std;

// where we store the actual components/data
template <class T> class StorageShelf
{
public:
    static StorageShelf<T>* GetInstance();
    T* mItems;

    T* GetPointerToArray() { return mItems; }

private:
    StorageShelf();
    ~StorageShelf();
};

template <class T> StorageShelf<T>* StorageShelf<T>::GetInstance()
{
    static StorageShelf<T> storageShelf;
    return &storageShelf;
}

template <class T> StorageShelf<T>::StorageShelf() : mItems(nullptr) { mItems = new T[MAX_NUM_ENTITIES](); }

template <class T> StorageShelf<T>::~StorageShelf() { delete[] mItems; }

// This is the magic about singletons
// Find the right shelf for the needed component
template <class T> static T* GetComponent(EntityID pEntityID)
{
    StorageShelf<T>* tNeededShelf = tNeededShelf->GetInstance();

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
    StorageShelf<T>* tFirstShelf = tFirstShelf->GetInstance();
    StorageShelf<U>* tSecondShelf = tSecondShelf->GetInstance();

    // Memcpy
    // TODO could take parameter of how many entities we have active
    memcpy(tSecondShelf->mItems, tFirstShelf->mItems, sizeof(T) * MAX_NUM_ENTITIES);
}
