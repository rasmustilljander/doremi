#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DirectXMath.h>

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
        A enum containing the different groups of sounds
        */
        enum class SoundGroup
        {
            Music,
            Effect,
            RecordAndAnalyse,
        };
        /**
        TODO docs
        */
        class AudioModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            Updates the audiomodule
            */
            virtual void Update() = 0;

            /**
            Loads sound from file (Used for debugging) TODOLH remove at release
            */
            virtual int LoadSound(const std::string& p_soundName, float p_minDistance, float p_maxDistance) = 0;
            /**
            Copies one sound to another. The second sound becomes a cropped version of the first. Cropping is made at the desired timepoint
            */
            virtual int CopySound(int p_soundIDSource, int p_soundIDDestination, float p_length) = 0;
            /**
            Returns true if currently recording
            */
            virtual bool IsRecording() = 0;
            /**
            Returns the elapsed time of a sound in milliseconds
            */
            virtual double GetSoundTimePointer(const int& p_channelID) = 0;
            /**
            Gets the length of a sound
            */
            virtual double GetSoundLength(const int& p_soundID) = 0;
            /**
            Plays a sound that is already in the system. chennelID should be < 0 the first time
            */
            virtual void PlayASound(int p_soundID, bool p_loop, int& p_channelID, const SoundGroup& p_soundGroup) = 0;
            /**
            Plays a sound on a specific channel
            */
            virtual void PlaySoundOnSpecificChannel(const int& p_soundID, bool p_loop, const int& p_channelID, const SoundGroup& p_soundGroup) = 0;
            /**
            Checks if the channel is playing
            */
            virtual bool GetChannelPlaying(const int& p_channelID) = 0;
            /**
            Stops recording from default recording driver
            */
            virtual void StopRecording() = 0;
            /**
            Sets the priority of the channel. 0 is highest 256 is lowest and 128 is default
            */
            virtual void SetPriority(const int& p_chanelID, const int& p_priority) = 0;
            /**
            TODO DOCS
            */
            virtual void Startup() = 0;

            /**
            Sets up the recording variables and returns the ID of the sound that will take care of the recording
            */
            virtual int SetupRecording(bool p_loop) = 0;

            /**
            Starts recording to the given soundvariable
            */
            virtual int StartRecording(int p_soundID, bool p_loopRec) = 0;

            /**
            Analyze a sound at the giving moment and returns the frequency
            */
            virtual float AnalyseSoundSpectrum(const int& p_channelID) = 0;
            /**
            Returns the pointer of the position in recording
            */
            virtual unsigned int GetRecordPointer() = 0;
            /**
            Sets up the 3D sound. parameters are the distance measurement for each parameter (1 for meters, 100 for centimeters, 3.28 for feet
            */
            virtual int Setup3DSound(float p_dopplerScale, float p_distanceFactor, float p_rollOffScale) = 0;

            /**
            Sets the position and velocity of a sound
            */
            virtual int SetSoundPositionAndVelocity(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_velocity, const int& p_channelID) = 0;

            /**
            Sets the listener position
            */
            virtual int SetListenerPos(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_forward, DirectX::XMFLOAT3 p_up) = 0;
            /**
            Sets the channels volume volume is 0 - 1, where  is loudest
            */
            virtual int SetVolumeOnChannel(const int& p_channelID, float p_volume) = 0;

            /**
            TODO docs
            */
            virtual void Shutdown() = 0;

            /**
            Returns true if Initialization was OK
            */
            virtual bool GetInitializationStatus() = 0;

            /**
            Stops the sound playing in the channel
            */
            virtual void StopSound(const int& p_channelID) = 0;

            /**
            Sets master volume 1 is full volume, 0 is no volume
            */
            virtual void SetMasterVolume(const float& p_volume) = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Audio::AudioModule* (*CREATE_AUDIO_MODULE)(const DoremiEngine::Core::SharedContext&);
AUDIO_DLL_EXPORT DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_context);
}