// Project specific
#include <GameCore.hpp>
#include <EntityComponent/EntityInterface.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <Manager/Manager.hpp>
#include <Manager/ExampleManager.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <EventHandler/EventHandler.hpp>
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
            
            // Create components
            ExampleComponent* t_exampleComponent = new ExampleComponent(5, 5);
            
            // Declare blueprint (do not reuse variables for more blueprints)
            EntityBlueprint t_entityBlueprint;

            // Set components of the blueprint
            t_entityBlueprint[ComponentType::Example] = t_exampleComponent;

            // Register blueprint to the appropriate bit mask (WARNING! Key will possibly change in the future)
            t_entityInterface->RegisterEntityBlueprint(Blueprints::ExampleEntity, t_entityBlueprint);
            
            // Create a couple of entities using the newly created blueprint
            for (size_t i = 0; i < 2; i++)
            {
                t_entityInterface->CreateEntity(Blueprints::ExampleEntity);
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
                libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::PHYSICS | DoremiEngine::Core::EngineModuleEnum::AUDIO); //Should add more modules to bitmask to initialize them

            EntityInterface* t_enitiyInterface = EntityInterface::GetInstance();
            
            // Create manager
            Manager* t_physicsManager = new ExampleManager(a);

            // Add manager to list of managers
            m_managers.push_back(t_physicsManager);

            // Create all entities (debug purposes only so far)
            GenerateWorld();
        }

        void GameCore::StartCore()
        {
            while (true)
            {
                // Have all managers update
                
				size_t length = m_managers.size();
                for (size_t i = 0; i < length; i++)
                {
                    m_managers[i]->Update(0.017);
                }
				EventHandler::GetInstance()->DeliverEvents();
            }
			
                //Game Loop
              

        }
    }
}