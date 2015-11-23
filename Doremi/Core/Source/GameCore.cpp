// Project specific
#include <GameCore.hpp>
#include <EntityComponent/EntityInterface.hpp>
#include <EntityComponent/Components/PhysicsComponent.hpp>
#include <Manager/PhysicsManager.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>

// Third party

// Standard libraries
#include <iostream> // Only debugging

using namespace std;
namespace Doremi
{
    namespace Core
    {
        GameCore::GameCore()
        {
        }
        GameCore::~GameCore()
        {
        }

        // Only for testing, should be removed!
        void GenerateWorld() 
        {
            Doremi::Core::EntityInterface* t_entityInterface = Doremi::Core::EntityInterface::GetInstance();
            PhysicsComponent* t_physicsComponent = new PhysicsComponent(5, 5);
            EntityBlueprint t_entityBlueprint;
            t_entityBlueprint[ComponentType::Physics] = t_physicsComponent;

            t_entityInterface->RegisterEntityBlueprint(Blueprints::FirstEntity, t_entityBlueprint);

            for (size_t i = 0; i < 2; i++)
            {
                t_entityInterface->CreateEntity(Blueprints::FirstEntity);
            }
        }


        void GameCore::Initialize()
        {
        


            // Load engine DLLs
            void* m_engineModule = DynamicLoader::LoadSharedLibrary("EngineCore.dll");

            if (m_engineModule == nullptr)
            {
                throw std::runtime_error("1Failed to load engine - please check your installation.");
            }

            INITIALIZE_ENGINE libInitializeEngine =
                (INITIALIZE_ENGINE)DynamicLoader::LoadProcess(m_engineModule, "InitializeEngine");

            if (libInitializeEngine == nullptr)
            {
                throw std::runtime_error("2Failed to load engine - please check your installation.");
            }
            const DoremiEngine::Core::SharedContext& a =
                libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::PHYSICS);
            //a.GetPhysicsModule().DoSomething();
            EntityInterface* t_enitiyInterface = EntityInterface::GetInstance();
            Manager* t_physicsManager = new PhysicsManager(a);
            m_managers.push_back(t_physicsManager);
            GenerateWorld();

        }
        void GameCore::StartCore()
        {


            while(true)
            {
                //Game Loop
                m_managers[0]->Update(0.017);
            }
        }
    }
}