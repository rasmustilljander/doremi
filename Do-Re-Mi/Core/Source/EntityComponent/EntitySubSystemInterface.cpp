#include <EntityComponent/EntitySubSystemInterface.hpp>
						 
#include <EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/StorageShelf.hpp>
#include <EntityComponent/ComponentTable.hpp>

EntitySubSystemInterface* EntitySubSystemInterface::m_singleton = nullptr;

EntitySubSystemInterface::EntitySubSystemInterface()
{

}

EntitySubSystemInterface::~EntitySubSystemInterface()
{

}

EntitySubSystemInterface* EntitySubSystemInterface::GetInstance()
{
	if (m_singleton == nullptr)
	{
		m_singleton = new EntitySubSystemInterface();
	}
	return m_singleton;
}

void EntitySubSystemInterface::RegisterEntityBlueprint(Blueprints p_blueprintID, EntityBlueprint p_blueprint)
{
	EntityFactory::GetInstance()->RegisterEntityTemplate(p_blueprintID, p_blueprint);
}

int EntitySubSystemInterface::GetLastEntityIndex()
{
	return EntityManager::GetInstance()->GetLastEntity();
}

bool EntitySubSystemInterface::HasComponents(EntityID p_id, int p_mask)
{
	return ComponentTable::GetInstance()->HasComponent(p_id, p_mask);
}

void EntitySubSystemInterface::CreateEntity(Blueprints p_blueprintID)
{
	EntityFactory::GetInstance()->CreateEntity(p_blueprintID);
}