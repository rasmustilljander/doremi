// Project specific
#include <EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/ComponentTable.hpp>
#include <EntityComponent/StorageShelf.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <EntityComponent/Components/AudioActiveComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/AIGroupComponent.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        EntityFactory* EntityFactory::mSingleton = nullptr;

        EntityFactory* EntityFactory::GetInstance()
        {
            if(mSingleton == nullptr)
            {
                mSingleton = new EntityFactory();
            }
            return mSingleton;
        }

        // TODORT Make sure this is called.
        void EntityFactory::ReleaseInstance()
        {
            delete mSingleton;
            mSingleton = nullptr;
        }

        EntityFactory::EntityFactory() {}

        EntityFactory::~EntityFactory() {}

        void EntityFactory::Initialize() {}

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
                // Add bitmask
                tComponentTable->AddComponent(tNewEntityID, (int)iter->first);

                // Check which component we copy
                if(iter->first == ComponentType::Example)
                {
                    memcpy(GetComponent<ExampleComponent>(tNewEntityID), iter->second, sizeof(ExampleComponent));
                }
                else if(iter->first == ComponentType::Example2)
                {
                    memcpy(GetComponent<Example2Component>(tNewEntityID), iter->second, sizeof(Example2Component));
                }
                else if(iter->first == ComponentType::Audio)
                {
                    memcpy(GetComponent<AudioComponent>(tNewEntityID), iter->second, sizeof(AudioComponent));
                }
                else if(iter->first == ComponentType::AudioActive)
                {
                    memcpy(GetComponent<AudioActiveComponent>(tNewEntityID), iter->second, sizeof(AudioActiveComponent));
                }
                else if(iter->first == ComponentType::Render)
                {
                    memcpy(GetComponent<RenderComponent>(tNewEntityID), iter->second, sizeof(RenderComponent));
                }
                else if(iter->first == ComponentType::Transform)
                {
                    memcpy(GetComponent<TransformComponent>(tNewEntityID), iter->second, sizeof(TransformComponent));
                }
                else if(iter->first == ComponentType::RigidBody)
                {
                    memcpy(GetComponent<RigidBodyComponent>(tNewEntityID), iter->second, sizeof(RigidBodyComponent));
                }
                else if(iter->first == ComponentType::PhysicalMaterial)
                {
                    memcpy(GetComponent<PhysicsMaterialComponent>(tNewEntityID), iter->second, sizeof(PhysicsMaterialComponent));
                }
                else if(iter->first == ComponentType::Movement)
                {
                    memcpy(GetComponent<MovementComponent>(tNewEntityID), iter->second, sizeof(MovementComponent));
                }
                else if(iter->first == ComponentType::Health)
                {
                    memcpy(GetComponent<HealthComponent>(tNewEntityID), iter->second, sizeof(HealthComponent));
                }
                else if(iter->first == ComponentType::Range)
                {
                    memcpy(GetComponent<RangeComponent>(tNewEntityID), iter->second, sizeof(RangeComponent));
                }
                else if(iter->first == ComponentType::PotentialField)
                {
                    memcpy(GetComponent<PotentialFieldComponent>(tNewEntityID), iter->second, sizeof(PotentialFieldComponent));
                }
                else if(iter->first == ComponentType::AIGroup)
                {
                    memcpy(GetComponent<AIGroupComponent>(tNewEntityID), iter->second, sizeof(AIGroupComponent));
                }
                else if(iter->first == ComponentType::Jump)
                {
                    memcpy(GetComponent<JumpComponent>(tNewEntityID), iter->second, sizeof(JumpComponent));
                }
            }
            return tNewEntityID;
        }
    }
}