#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/ComponentTable.hpp>

EntityManager* EntityManager::mSingleton = nullptr;

EntityManager* EntityManager::GetInstance()
{
	if (mSingleton == nullptr)
	{
		mSingleton = new EntityManager();
	}
	return mSingleton;
}

EntityManager::EntityManager()
{
	mNextSlot = 0;
}


EntityManager::~EntityManager()
{
}

EntityID EntityManager::AddEntity()
{
	EntityID tIDToUse;

	//if any free slots we use them
	if (mFreeEntitySlots.size() != 0)
	{
		tIDToUse = mFreeEntitySlots.back();
		mFreeEntitySlots.pop_back();
	}
	else
	{
		tIDToUse = mNextSlot;
		mNextSlot++;
	}
	return tIDToUse;
}

void EntityManager::RemoveEntity(int pEntityID)
{
	//Remove from table
	ComponentTable* tCompTable = tCompTable->GetInstance();
	tCompTable->RemoveEntity(pEntityID);

	std::list<EntityID>::iterator findIter = std::find(mFreeEntitySlots.begin(), mFreeEntitySlots.end(), pEntityID);

	if (findIter == mFreeEntitySlots.end())
	{
		mFreeEntitySlots.push_back(pEntityID);
	}

}
