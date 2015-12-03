#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

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
        class AudioModule : public DoremiEngine::Core::EngineModule
        {
        public:

            //Updates the audiomodule
            virtual int Update() = 0;

            //Loads sound from file (Used for debugging) TODOLH remove at release
            virtual size_t LoadSound(const std::string& p_soundName) = 0;

            //Plays a sound that is already in the system
            virtual void PlaySound(size_t p_soundID, bool p_loop) = 0;

            //Initialize code
            virtual void Startup() = 0;

            //Sets up the recording variables and returns the ID of the sound that will take care of the recording
            virtual size_t SetupRecording() = 0;

            //Starts recording to the given soundvariable
            virtual int StartRecording() = 0;

            /**
            TODO docs
            */
            virtual void Shutdown() = 0;
        };
    }
}

extern "C" {
    typedef DoremiEngine::Audio::AudioModule* (*CREATE_AUDIO_MODULE)(const DoremiEngine::Core::SharedContext&);
    AUDIO_DLL_EXPORT DoremiEngine::Audio::AudioModule*
        CreateAudioModule(const DoremiEngine::Core::SharedContext& p_context);
}