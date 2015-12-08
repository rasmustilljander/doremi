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

            /**
                TORORT docs
            */
            void Update() override;

            /**
                TODO docs
            */
            void Shutdown() override;
            /**
            TODO docs
            */

            size_t LoadSound(const std::string& p_soundName, float p_minDistance, float p_maxDistance) override;

            void PlayASound(size_t p_soundID, bool p_loop, size_t p_channelID) override;

            size_t SetupRecording(bool p_loop) override;

            int StartRecording(size_t p_soundID, bool p_loopRec, size_t p_channelID) override;

            float AnalyseSoundSpectrum(const size_t& p_channelID) override;

            int Setup3DSound(float p_dopplerScale, float p_distanceFactor, float p_rollOffScale) override;

            int SetSoundPositionAndVelocity(float p_posx, float p_posy, float p_posz,
                float p_velx, float p_vely, float p_velz, const size_t& p_channelID) override;
            
            int SetListenerPos(float p_posx, float p_posy, float p_posz, float p_forwardx, float p_forwardy, float p_forwardz,
                float p_upx, float p_upy, float p_upz) override;

            int SetVolumeOnChannel(const size_t& p_channelID, float p_volume) override;
        private:
            void ERRCHECK(const FMOD_RESULT& p_Result);


            FMOD::System              *m_fmodSystem;
            std::vector<FMOD::Sound*>  m_fmodSoundBuffer;
            std::vector<FMOD::Channel*>m_fmodChannel;
            FMOD::Channel *m_background = 0;
            FMOD::Channel *m_record = 0;
            FMOD::Channel *m_enemy = 0;

            FMOD_RESULT                m_fmodResult;
            int                        m_fmodKey;
            unsigned int               m_fmodVersion;
            int                        time = 0;

            const Core::SharedContext& m_sharedContext;
            float m_distanceFactor = 0;
            bool m_recordingStarted = false;

            static const int m_outputRate = 4800;
            static const int m_spectrumSize = 8192;
            const float m_spectrumRange = ((float)m_outputRate / 2.0f);
            const float m_binSize = m_spectrumRange / ((float)m_spectrumSize);
        };
    }
}
