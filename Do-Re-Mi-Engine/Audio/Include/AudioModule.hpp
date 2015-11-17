#include <Do-Re-Mi-Engine/Core/Include/Subsystem/EngineSubsystem.hpp>
#include <Do-Re-Mi-Engine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define AUDIO_DLL_EXPORT __declspec(dllexport)
#else
#define AUDIO_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Audio
    {
        /**
        TODO docs
        */
        class AudioModule : public DoremiEngine::Core::EngineSubsystem
        {
            public:
            virtual void DoFunction() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Audio::AudioModule *(*CREATE_AUDIO_MODULE)(DoremiEngine::Core::SharedContext);
AUDIO_DLL_EXPORT DoremiEngine::Audio::AudioModule *CreateSoundInterface(DoremiEngine::Core::SharedContext p_context);
}