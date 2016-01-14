#pragma once
// Project specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

using namespace std;

// where we store the actual components/data
template <class T> class StorageShelf
{
public:
    static StorageShelf<T>* GetInstance();
    T mItems[MAX_NUM_ENTITIES];

    // TODOCM bad solution to swap pointers of singleton, fix when there's time
    void SetSingleton(void* p_pointer);

private:
    StorageShelf();
    ~StorageShelf();

    static StorageShelf<T>* mSingleton;
};


template <class T> StorageShelf<T>* StorageShelf<T>::mSingleton = nullptr;

template <class T> StorageShelf<T>* StorageShelf<T>::GetInstance()
{
    if(mSingleton == nullptr)
    {
        mSingleton = new StorageShelf<T>();
    }

    return mSingleton;
}

template <class T> void StorageShelf<T>::SetSingleton(void* p_pointer) { mSingleton = (StorageShelf<T>*)p_pointer; }

template <class T> StorageShelf<T>::StorageShelf() {}

template <class T> StorageShelf<T>::~StorageShelf() {}


// This is the magic about singletons
// Find the right shelf for the needed component
template <class T> static T* GetComponent(EntityID pEntityID)
{
    StorageShelf<T>* tNeededShelf = tNeededShelf->GetInstance();

    return &tNeededShelf->mItems[pEntityID];
}

template <class T, class U> static void SwapShelf()
{
    if(sizeof(T) != sizeof(U))
    {
        std::runtime_error("Attempting to swap two different sized shelfs!");
    }

    // Get Pointers
    StorageShelf<T>* tFirstShelf = tFirstShelf->GetInstance();
    StorageShelf<U>* tSecondShelf = tSecondShelf->GetInstance();

    // Set pointers
    tFirstShelf->SetSingleton(tSecondShelf);
    tSecondShelf->SetSingleton(tFirstShelf);
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
    memcpy(tSecondShelf, tFirstShelf, sizeof(T) * MAX_NUM_ENTITIES);
}
