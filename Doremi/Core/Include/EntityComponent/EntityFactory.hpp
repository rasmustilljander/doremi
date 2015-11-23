#pragma once
// Project Specific
#include <EntityComponent/Constants.hpp>

// Standard libraries
#include <string>
#include <map>
#include <vector>


using namespace std;
namespace Doremi
{
    namespace Core
    {
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
    }
}