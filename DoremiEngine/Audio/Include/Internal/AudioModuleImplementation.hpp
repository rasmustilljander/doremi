#pragma once
#include <AudioModule.hpp>
#include <FMod\fmod.hpp>
#include <FMod\fmod_errors.h>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Audio
    {
        class AudioModuleImplementation : public AudioModule
        {
            public:
            /**
                TODO docs
            */
            AudioModuleImplementation(const Core::SharedContext& p_sharedContext);
            /**
                TODO docs
            */
            virtual ~AudioModuleImplementation();
            /**
                TODO docs
            */
            void Startup() override;

            int Update() override;
            /**
                TODO docs
            */
            void Shutdown() override;
            /**
            TODO docs
            */
            float DoFunction(float a, float b) override;

            private:
            void LoadSound(const std::string& p_soundName) override;
            void ERRCHECK(const FMOD_RESULT& p_Result);
            
            FMOD::System        *m_system;
            FMOD::Sound         *m_sound;
            FMOD::Channel       *m_channel = 0;
            FMOD_RESULT         m_fmodResult;
            int                 m_fmodKey;
            unsigned int        m_fmodVersion;
            const Core::SharedContext& m_sharedContext;



        };
    }
}