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
            /**
                TODO remove this test method
            */
            virtual float DoFunction(float a, float b) = 0;

            /**
            TODO docs
            */
            
            virtual int Update() = 0;


            //Loads sound from file (Used for debugging) TODOLH remove at release
            virtual void LoadSound(const std::string& p_soundName) = 0;

            virtual void Startup() = 0;

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