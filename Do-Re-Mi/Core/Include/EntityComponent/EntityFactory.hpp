#pragma once
#include <string>
#include <map>
#include <EntityComponent/Constants.hpp>
#include <vector>

using namespace std;

class EntityFactory
{
public:


	static EntityFactory* GetInstance();

	void Initialize();
	void RegisterEntityTemplate(Blueprints p_blueprintID, EntityBlueprint pComponents);
	EntityID CreateEntity(Blueprints p_blueprintID);



private:
	EntityFactory();
	~EntityFactory();
	static EntityFactory* mSingleton;



	std::map<Blueprints, EntityBlueprint> mEntityBlueprints;
};

