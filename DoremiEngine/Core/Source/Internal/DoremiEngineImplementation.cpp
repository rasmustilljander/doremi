#include <Internal/DoremiEngineImplementation.hpp>
#include <Subsystem/EngineModuleEnum.hpp>

#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>

#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>

#include <Internal/SharedContextImplementation.hpp>
#include <Windows.h>
namespace DoremiEngine
{
    namespace Core
    {
        DoremiEngineImplementation::DoremiEngineImplementation()
            : m_audioLibrary(nullptr),
              m_graphicLibrary(nullptr),
              m_memoryLibrary(nullptr),
              m_networkLibrary(nullptr),
              m_physicsLibrary(nullptr),
              m_scriptLibrary(nullptr),
              m_audioModule(nullptr),
              m_graphicModule(nullptr),
              m_memoryModule(nullptr),
              m_networkModule(nullptr),
              m_physicsModule(nullptr),
              m_scriptModule(nullptr)
        {
        }

        DoremiEngineImplementation::~DoremiEngineImplementation()
        {
            if(m_audioModule != nullptr)
            {
                delete m_audioModule;
            }

            if(m_graphicModule != nullptr)
            {
                delete m_graphicModule;
            }

            //    if (m_memoryModule != nullptr)
            //    {
            //        delete m_memoryModule;
            //    }

            // TODORT
            //    if (m_networkModule != nullptr)
            //    {
            //        delete m_networkModule;
            //    }

            if(m_physicsModule != nullptr)
            {
                delete m_physicsModule;
            }

            //    if (m_scriptModule != nullptr)
            //    {
            //        delete m_scriptModule;
            //    }


            if(m_audioLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_audioLibrary);
            }
            if(m_graphicLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_graphicLibrary);
            }
            if(m_memoryLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_memoryLibrary);
            }
            if(m_networkLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_networkLibrary);
            }
            if(m_physicsLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_physicsLibrary);
            }
            if(m_scriptLibrary != nullptr)
            {
                DynamicLoader::FreeSharedLibrary(m_scriptLibrary);
            }
        }

        SharedContext& DoremiEngineImplementation::Start(const size_t& p_flags)
        {
            m_sharedContext = new SharedContextImplementation();
            m_sharedContext->SetCoreModule(this);
            BuildWorkingDirectory(*m_sharedContext);

            if((p_flags & EngineModuleEnum::AUDIO) == EngineModuleEnum::AUDIO)
            {
                LoadAudioModule(*m_sharedContext);
            }

            if((p_flags & EngineModuleEnum::GRAPHIC) == EngineModuleEnum::GRAPHIC)
            {
                LoadGraphicModule(*m_sharedContext);
            }

            if((p_flags & EngineModuleEnum::NETWORK) == EngineModuleEnum::NETWORK)
            {
                LoadNetworkModule(*m_sharedContext);
            }

            if((p_flags & EngineModuleEnum::PHYSICS) == EngineModuleEnum::PHYSICS)
            {
                LoadPhysicsModule(*m_sharedContext);
            }

            if((p_flags & EngineModuleEnum::SCRIPT) == EngineModuleEnum::SCRIPT)
            {
                LoadScriptModule(*m_sharedContext);
            }

            return *m_sharedContext;
        }

        void DoremiEngineImplementation::Stop()
        {
            if(m_audioModule != nullptr)
            {
                m_audioModule->Shutdown();
            }

            if(m_graphicModule != nullptr)
            {
                m_graphicModule->Shutdown();
            }

            //    if (m_memoryModule != nullptr)
            //    {
            //        m_memoryModule->Shutdown();
            //    }

            // TODORT
            //    if (m_networkModule != nullptr)
            //    {
            //        m_networkModule->Shutdown();
            //    }

            if(m_physicsModule != nullptr)
            {
                m_physicsModule->Shutdown();
            }

            //    if (m_scriptModule != nullptr)
            //    {
            //        m_scriptModule->Shutdown();
            //    }
        }

        void DoremiEngineImplementation::BuildWorkingDirectory(SharedContextImplementation& o_sharedContext)
        {
            char path[254];
            GetModuleFileNameA(NULL, path, 254);
            std::string* directoryPath = new std::string(path);            
            o_sharedContext.SetWorkingDirectory(directoryPath->substr(0, directoryPath->length() - 10));
        }

        void DoremiEngineImplementation::LoadAudioModule(SharedContextImplementation& o_sharedContext)
        {
            m_audioLibrary = DynamicLoader::LoadSharedLibrary("Audio.dll");

            if(m_audioLibrary != nullptr)
            {
                CREATE_AUDIO_MODULE functionCreateAudioModule =
                (CREATE_AUDIO_MODULE)DynamicLoader::LoadProcess(m_audioLibrary,
                                                                "CreateAudioModule");
                if(functionCreateAudioModule != nullptr)
                {
                    m_audioModule =
                    static_cast<Audio::AudioModule*>(functionCreateAudioModule(o_sharedContext));
                    m_audioModule->Startup();
                    o_sharedContext.SetAudioModule(m_audioModule);
                }
                else
                {
                    // TODO logger
                }
            }
            else
            {
                // TODO logger
            }
        }

        void DoremiEngineImplementation::LoadGraphicModule(SharedContextImplementation& o_sharedContext)
        {
            m_graphicLibrary = DynamicLoader::LoadSharedLibrary("Graphic.dll");

            if(m_graphicLibrary != nullptr)
            {
                CREATE_GRAPHIC_MODULE functionCreateGraphicModule =
                (CREATE_GRAPHIC_MODULE)DynamicLoader::LoadProcess(m_graphicLibrary,
                                                                  "CreateGraphicModule");
                if(functionCreateGraphicModule != nullptr)
                {
                    m_graphicModule =
                    static_cast<Graphic::GraphicModule*>(functionCreateGraphicModule(o_sharedContext));
                    m_graphicModule->Startup();
                    o_sharedContext.SetGraphicModule(m_graphicModule);
                }
                else
                {
                    // TODO logger
                }
            }
            else
            {
                // TODO logger
            }
        }

        void DoremiEngineImplementation::LoadMemoryModule(SharedContextImplementation& o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadNetworkModule(SharedContextImplementation& o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadPhysicsModule(SharedContextImplementation& o_sharedContext)
        {
            m_physicsLibrary = DynamicLoader::LoadSharedLibrary("Physics.dll");

            if(m_physicsLibrary != nullptr)
            {
                CREATE_PHYSICS_MODULE functionCreatePhysicsModule =
                (CREATE_PHYSICS_MODULE)DynamicLoader::LoadProcess(m_physicsLibrary,
                                                                  "CreatePhysicsModule");
                if(functionCreatePhysicsModule != nullptr)
                {
                    m_physicsModule =
                    static_cast<Physics::PhysicsModule*>(functionCreatePhysicsModule(o_sharedContext));
                    m_physicsModule->Startup();
                    o_sharedContext.SetPhysicsModule(m_physicsModule);
                }
                else
                {
                    // TODO logger
                }
            }
            else
            {
                // TODO logger
            }
        }

        void DoremiEngineImplementation::LoadScriptModule(SharedContextImplementation& o_sharedContext)
        {
        }
    }
}

namespace
{
    DoremiEngine::Core::DoremiEngineImplementation* engine = nullptr;
}

const DoremiEngine::Core::SharedContext& StartEngine(const size_t& p_flags)
{
    if(engine == nullptr)
    {
        engine = new DoremiEngine::Core::DoremiEngineImplementation();
        engine->Start(p_flags);
        return engine->GetSharedContext();
    }
    throw std::exception("Engine has already been started, it must be stopped before it can be started again.");
}

void StopEngine()
{
    engine->Stop();
    delete engine;
    engine = nullptr;
}