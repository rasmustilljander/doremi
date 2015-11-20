#include <Do-Re-Mi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <Do-Re-Mi/Core/Include/EntityComponent/EntityManager.hpp>
#include <Do-Re-Mi/Core/Include/EntityComponent/ComponentTable.hpp>
#include <EntityComponent/StorageShelf.hpp>

EntityFactory* EntityFactory::mSingleton = nullptr;

EntityFactory* EntityFactory::GetInstance()
{
    if(mSingleton == nullptr)
    {
        mSingleton = new EntityFactory();
    }
    return mSingleton;
}

EntityFactory::EntityFactory()
{
}


EntityFactory::~EntityFactory()
{
}


void EntityFactory::Initialize()
{
}

void EntityFactory::RegisterEntityTemplate(Blueprints p_blueprintID, EntityBlueprint pComponents)
{
    mEntityBlueprints[p_blueprintID] = pComponents;
}

EntityID EntityFactory::CreateEntity(Blueprints p_blueprintID)
{
    EntityBlueprint tComponentMap = mEntityBlueprints[p_blueprintID];
    EntityManager* tEntityManager = tEntityManager->GetInstance();
    ComponentTable* tComponentTable = tComponentTable->GetInstance();

    // create a new ID
    EntityID tNewEntityID = tEntityManager->AddEntity();

    // copy components
    for(EntityBlueprint::iterator iter = tComponentMap.begin(); iter != tComponentMap.end(); ++iter)
    {
        tComponentTable->AddComponent(tNewEntityID, (int)iter->first);

        // if (iter->first == ComponentType::Physics)
        //{
        //	memcpy(GetComponent<PhysicsComponent>(tNewEntityID), iter->second,
        // sizeof(PhysicsComponent));
        //}
    }

    return tNewEntityID;
}