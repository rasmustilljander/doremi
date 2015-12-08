// Project specific
#include <GameCore.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <Manager/Manager.hpp>
#include <Manager/ExampleManager.hpp>
#include <Manager/AudioManager.hpp>
#include <Manager/Network/ClientNetworkManager.hpp>
#include <Manager/Network/ServerNetworkManager.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EventHandler/EventHandler.hpp>
#include <InputHandler.hpp>

#include <string>


// Third party

// Standard libraries
#include <iostream> // Only debugging

using namespace std;
namespace Doremi
{
    namespace Core
    {
        GameCore::GameCore() { LoadEngineLibrary(); }

        GameCore::~GameCore()
        {
            m_stopEngineFunction();
            DynamicLoader::FreeSharedLibrary(m_engineLibrary);
        }


        // Only for testing, should be removed! TODO
        void GenerateWorld()
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create components
            ExampleComponent* t_exampleComponent = new ExampleComponent(5, 5);
            Example2Component* t_example2Component = new Example2Component();

            // Declare blueprint (do not reuse variables for more blueprints)
            EntityBlueprint t_entityBlueprint;

            // Set components of the blueprint
            t_entityBlueprint[ComponentType::Example] = t_exampleComponent;
            t_entityBlueprint[ComponentType::Example2] = t_example2Component;

            // Register blueprint to the appropriate bit mask (WARNING! Key will possibly change in
            // the future)
            t_entityHandler.RegisterEntityBlueprint(Blueprints::ExampleEntity, t_entityBlueprint);

            // Create a couple of entities using the newly created blueprint
            for(size_t i = 0; i < 2; i++)
            {
                t_entityHandler.CreateEntity(Blueprints::ExampleEntity);
            }
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


        void GameCore::InitializeClient()
        {
            InputHandler::GetInstance()->Initialize();//TODOEA NUGGET SNÄASDLS Tack som, fan.
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

            const DoremiEngine::Core::SharedContext& sharedContext =
                libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::ALL ^ DoremiEngine::Core::EngineModuleEnum::GRAPHIC);

            EntityHandler& t_entityHandler = EntityHandler::GetInstance();


            // Lucas Testkod


            sharedContext.GetAudioModule().Startup();
            sharedContext.GetAudioModule().Setup3DSound(1.0f, 1.0f, 0.1f);
            sharedContext.GetAudioModule().SetListenerPos(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

            EntityBlueprint t_audioBlueprint;
            AudioComponent* t_audioComponent = new AudioComponent();
            t_audioComponent->soundID = sharedContext.GetAudioModule().LoadSound("Sounds/Test sounds/2000hz 10 amp  db.wav", 0.5f, 5000.0f);
            sharedContext.GetAudioModule().LoadSound("Sounds/Test sounds/2000hz 10 amp  db.wav", 0.5f, 5000.0f);
            // size_t t_soundNumber = sharedContext.GetAudioModule().LoadSound("Sounds/329842__zagi2__smooth-latin-loop.wav", 0.5f, 5000.0f);
            // size_t t_soundNumber = sharedContext.GetAudioModule().LoadSound("Sounds/Test sounds/High to low pitch.wav", 0.5f, 5000.0f);
            // size_t t_soundNumber = sharedContext.GetAudioModule().LoadSound("Sounds/Test sounds/1000hz.wav", 0.5f, 5000.0f);
            // t_audioBlueprint[ComponentType::Audio] =
            sharedContext.GetAudioModule().PlayASound(t_audioComponent->soundID, true, 0);
            sharedContext.GetAudioModule().SetVolumeOnChannel(0, 0);
            size_t t = sharedContext.GetAudioModule().SetupRecording(true);
            sharedContext.GetAudioModule().StartRecording(t, true, 1);
            sharedContext.GetAudioModule().SetSoundPositionAndVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);

            Manager* t_audioManager = new AudioManager(sharedContext);
            m_managers.push_back(t_audioManager);
            // Lucas Testkod slut
            ////////////////Example only////////////////
            // Create manager

            // Manager* t_physicsManager = new ExampleManager(sharedContext);
            Manager* t_clientNetworkManager = new ClientNetworkManager(sharedContext);


            // Add manager to list of managers
            // m_managers.push_back(t_physicsManager);
            m_managers.push_back(t_clientNetworkManager);

            GenerateWorld();
            ////////////////End Example////////////////
        }

        void GameCore::InitializeServer()
        {
            START_ENGINE libInitializeEngine = (START_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StartEngine");

            if (libInitializeEngine == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            m_stopEngineFunction = (STOP_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StopEngine");

            if (m_stopEngineFunction == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            const DoremiEngine::Core::SharedContext& sharedContext = libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::NETWORK);

            EntityHandler& t_entityHandler = EntityHandler::GetInstance();


            //Lucas Testkod
            //sharedContext.GetAudioModule().Startup();
            //sharedContext.GetAudioModule().Setup3DSound(1.0f, 1.0f, 0.1f);
            //sharedContext.GetAudioModule().SetListenerPos(0.0f , 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
            ////size_t t_soundNumber = a.GetAudioModule().LoadSound("Sounds/329842__zagi2__smooth-latin-loop.wav", 0.5f, 5000.0f);
            ////size_t t_soundNumber = a.GetAudioModule().LoadSound("Sounds/Test sounds/High to low pitch.wav", 0.5f, 5000.0f);
            ////size_t t_soundNumber = a.GetAudioModule().LoadSound("Sounds/Test sounds/1000hz.wav", 0.5f, 5000.0f);
            ////a.GetAudioModule().PlayASound(t_soundNumber, true, 0);
            //size_t t = sharedContext.GetAudioModule().SetupRecording(true);
            //sharedContext.GetAudioModule().StartRecording(t, true, 0);
            //sharedContext.GetAudioModule().SetSoundPosAndVel(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);

            //Manager* t_audioManager = new AudioManager(sharedContext);
            //m_managers.push_back(t_audioManager);
            //Lucas Testkod slut

            ////////////////Example only////////////////
            // Create manager

            Manager* t_physicsManager = new ExampleManager(sharedContext);
            Manager* t_serverNetworkManager = new ServerNetworkManager(sharedContext);


            // Add manager to list of managers
            m_managers.push_back(t_physicsManager);
            m_managers.push_back(t_serverNetworkManager);

            GenerateWorld();
            ////////////////End Example////////////////
        }

        void GameCore::StartCore()
        {
            while(true)
            {
                // Have all managers update

                size_t length = m_managers.size();
                for(size_t i = 0; i < length; i++)
                {
                    m_managers.at(i)->Update(0.017);
                }
                EventHandler::GetInstance()->DeliverEvents();
                //InputHandler::GetInstance()->Update(sharedContext);
                //TODOEA Langa update här för input
            }
        }
    }
}