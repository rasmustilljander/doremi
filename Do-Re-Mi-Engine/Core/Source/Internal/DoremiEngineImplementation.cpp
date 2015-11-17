#include <Internal/DoremiEngineImplementation.hpp>
#include <Subsystem/EngineModuleEnum.hpp>

#include <Do-Re-Mi-Engine/Audio/Include/AudioModule.hpp>

#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>

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

        SharedContext &DoremiEngineImplementation::Initialize(const size_t &p_flags)
        {
            m_sharedContext = new SharedContext();

            m_sharedContext->core = this;

            if((p_flags & EngineModuleEnum::AUDIO) == EngineModuleEnum::AUDIO)
            {
                LoadAudioModule(*m_sharedContext);
            }

            if((p_flags & EngineModuleEnum::GRAPHIC) == EngineModuleEnum::GRAPHIC)
            {
                LoadGraphicModule(*m_sharedContext);
            }

            if((p_flags & EngineModuleEnum::MEMORY) == EngineModuleEnum::MEMORY)
            {
                LoadMemoryModule(*m_sharedContext);
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

        void DoremiEngineImplementation::LoadAudioModule(SharedContext &o_sharedContext)
        {
            //            m_audioModule =
            //            (DoremiEngine::Audio::AudioModule*)DynamicLoader::LoadSharedLibrary("Audio.dll");
        }

        void DoremiEngineImplementation::LoadGraphicModule(SharedContext &o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadMemoryModule(SharedContext &o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadNetworkModule(SharedContext &o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadPhysicsModule(SharedContext &o_sharedContext)
        {
        }

        void DoremiEngineImplementation::LoadScriptModule(SharedContext &o_sharedContext)
        {
        }
    }
}

const DoremiEngine::Core::SharedContext &InitializeModule(const size_t &p_flags)
{
    DoremiEngine::Core::DoremiEngine *engine = new DoremiEngine::Core::DoremiEngineImplementation();
    return engine->GetSharedContext();
}