#include <Internal/DoremiEngineImplementation.hpp>
#include <Subsystem/EngineModuleEnum.hpp>

#include <Do-Re-Mi-Engine/Audio/Include/AudioModule.hpp>

#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>

#include <Internal/SharedContextImplementation.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        DoremiEngineImplementation::DoremiEngineImplementation()
        {
        }

        DoremiEngineImplementation::~DoremiEngineImplementation()
        {
        }

        SharedContext& DoremiEngineImplementation::Initialize(const size_t& p_flags)
        {
            m_sharedContext = new SharedContextImplementation();

            m_sharedContext->SetCoreModule(this);

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
        }

        void DoremiEngineImplementation::LoadMemoryModule(SharedContextImplementation& o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadNetworkModule(SharedContextImplementation& o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadPhysicsModule(SharedContextImplementation& o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadScriptModule(SharedContextImplementation& o_sharedContext)
        {
        }
    }
}

const DoremiEngine::Core::SharedContext& InitializeEngine(const size_t& p_flags)
{
    DoremiEngine::Core::DoremiEngineImplementation* engine =
    new DoremiEngine::Core::DoremiEngineImplementation();
    engine->Initialize(p_flags);
    return engine->GetSharedContext();
}