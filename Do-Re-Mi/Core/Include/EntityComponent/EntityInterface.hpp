#pragma once
#include <EntityComponent/Constants.hpp>
#include <EntityComponent/ComponentTable.hpp>
#include <EntityComponent/StorageShelf.hpp>

class EntityInterface
{
public:
	static EntityInterface* GetInstance();

	// Registers a blueprint for entity creation with an ID
	void RegisterEntityBlueprint(Blueprints p_blueprintID, EntityBlueprint p_blueprint);
	int GetLastEntityIndex();

	// Checks if the entity specified has the entities in the mask. Mask has to be created manually
	bool HasComponents(EntityID p_id, int p_mask);

	//
	void CreateEntity(Blueprints p_blueprintID);

	// Returns desired component. Example: GetComponentFromStorage<ComponentName>(id);
	template<class T>
	T* GetComponentFromStorage(EntityID p_id)
	{
		return GetComponent<T>(p_id);
	}

private:
	EntityInterface();
	~EntityInterface();
	static EntityInterface* m_singleton;

};

//template<class T>
//static T* EntitySubSystemInterface::GetComponentFromStorage(EntityID p_id)
//{
//	return GetComponent<T>(p_id);
//}
