// Project specific
#include <GameCore.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/AIGroupComponent.hpp>
#include <Manager/Manager.hpp>
#include <Manager/ExampleManager.hpp>
#include <Manager/AudioManager.hpp>
#include <Manager/Network/ClientNetworkManager.hpp>
#include <Manager/Network/ServerNetworkManager.hpp>
#include <Manager/GraphicManager.hpp>
#include <Manager/RigidTransformSyncManager.hpp>
#include <Manager/CharacterControlSyncManager.hpp>
#include <Manager/TriggerManager.hpp>
#include <Manager/AI/AIPathManager.hpp>
#include <Manager/LightManager.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EventHandler/EventHandler.hpp>
#include <InputHandler.hpp>
#include <EntityComponent/Components/AudioActiveComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PlayerComponent.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <PlayerHandler.hpp>
#include <Manager/MovementManagerServer.hpp>
#include <AudioHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EventHandler/Events/PlayerCreationEvent.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <InterpolationHandler.hpp>
#include <LevelLoader.hpp>

#include <string>

// TODOCM remove for better timer?
#include <Windows.h>
#include <chrono>

// Third party

// Standard libraries
#include <iostream> // Only debugging

using namespace std;
namespace Doremi
{
    namespace Core
    {
        GameCore::GameCore() : m_stopEngineFunction(nullptr), m_engineLibrary(nullptr) { LoadEngineLibrary(); }

        GameCore::~GameCore()
        {
            if(m_stopEngineFunction != nullptr)
            {
                m_stopEngineFunction();
            }
            if(m_engineLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_engineLibrary);
            }
            // TODORT Release the Managers... Waiting for refactor.
        }

        void GameCore::LoadEngineLibrary()
        {
            // Load engine DLL
            m_engineLibrary = DynamicLoader::LoadSharedLibrary("EngineCore.dll");


            if(m_engineLibrary == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("1Failed to load engine - please check your installation.");
            }
        }

        const DoremiEngine::Core::SharedContext& GameCore::InitializeEngine(const size_t& p_engineModulesToStart)
        {
            START_ENGINE libInitializeEngine = (START_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StartEngine");

            if(libInitializeEngine == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            m_stopEngineFunction = (STOP_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StopEngine");

            if(m_stopEngineFunction == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            const DoremiEngine::Core::SharedContext& context = libInitializeEngine(p_engineModulesToStart);
            m_sharedContext = &context;
            return context;
        }
    }
}