#pragma once
// Project specific
#include <EntityComponent/Constants.hpp>

using namespace std;

// where we store the actual components/data
template <class T> class StorageShelf
{
    public:
    static StorageShelf<T>* GetInstance();
    T mItems[MAX_NUM_ENTITIES];


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


template <class T> StorageShelf<T>::StorageShelf() {}

template <class T> StorageShelf<T>::~StorageShelf() {}


// This is the magic about singletons
// Find the right shelf for the needed component
template <class T> static T* GetComponent(EntityID pEntityID)
{
    StorageShelf<T>* tNeededShelf = tNeededShelf->GetInstance();

    return &tNeededShelf->mItems[pEntityID];
}
