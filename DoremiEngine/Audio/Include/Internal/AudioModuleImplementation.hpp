#pragma once
#include <AudioModule.hpp>
#include <FMod\fmod.hpp>
#include <FMod\fmod_errors.h>
#include <vector>

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
            size_t LoadSound(const std::string& p_soundName) override;

            void PlaySound(size_t p_soundID, bool p_loop) override;

            size_t SetupRecording() override;

        private:
            void ERRCHECK(const FMOD_RESULT& p_Result);

            FMOD::System            *m_fmodSystem;
            std::vector<FMOD::Sound*> m_fmodSoundBuffer;
            FMOD::Channel           *m_fmodChannel = 0;
            FMOD_RESULT              m_fmodResult;
            int                      m_fmodKey;
            unsigned int             m_fmodVersion;
            const Core::SharedContext& m_sharedContext;
        };
    }
}
