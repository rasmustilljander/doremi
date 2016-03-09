#pragma once
#include <AudioModule.hpp>
#include <FMod\fmod.hpp>
#include <FMod\fmod_errors.h>
#include <vector>
#include <DirectXMath.h>

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
            unsigned int GetRecordPointer() override;

            int CopySound(int p_soundIDToCopy, int p_soundIDDestination, float p_length) override;

            bool IsRecording() override;

            double GetSoundTimePointer(const int& p_channelID) override;

            double GetSoundLength(const int& p_soundID) override;

            int LoadSound(const std::string& p_soundName, float p_minDistance, float p_maxDistance) override;

            void PlayASound(int p_soundID, bool p_loop, int& p_channelID) override;

            bool GetChannelPlaying(const int& p_channelID) override;

            int SetupRecording(bool p_loop) override;

            int StartRecording(int p_soundID, bool p_loopRec) override;

            float AnalyseSoundSpectrum(const int& p_channelID) override;

            int Setup3DSound(float p_dopplerScale, float p_distanceFactor, float p_rollOffScale) override;

            int SetSoundPositionAndVelocity(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_velocity, const int& p_channelID) override;

            int SetListenerPos(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_forward, DirectX::XMFLOAT3 p_up) override;

            int SetVolumeOnChannel(const int& p_channelID, float p_volume) override;

            bool GetInitializationStatus() override;

            void StopRecording() override;

            void SetPriority(const int& p_chanelID, const int& p_priority) override;

            void PlaySoundOnSpecificChannel(const int& p_soundID, bool p_loop, const int& p_channelID) override;

            void StopSound(const int& p_channelID) override;

        private:
            void ERRCHECK(const FMOD_RESULT& p_Result);


            FMOD::System* m_fmodSystem;
            std::vector<FMOD::Sound*> m_fmodSoundBuffer;
            std::vector<FMOD::Channel*> m_fmodChannel;
            FMOD::Channel* m_background = 0;
            FMOD::Channel* m_record = 0;
            FMOD::Channel* m_enemy = 0;

            FMOD_RESULT m_fmodResult;
            int m_fmodKey;
            unsigned int m_fmodVersion;
            int time = 0;
            bool m_initOK = false;

            const Core::SharedContext& m_sharedContext;
            float m_distanceFactor = 0;
            bool m_recordingStarted = false;

            static const int m_outputRate = 44100; // 4800 TODOLH Om freq analys buggar kolla hääärrrr!!!! fittunge (för ctrl+f)
            static const int m_spectrumSize = 8192;
            const float m_spectrumRange = ((float)m_outputRate / 2.0f);
            const float m_binSize = m_spectrumRange / ((float)m_spectrumSize);
        };
    }
}
