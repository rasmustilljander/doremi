#pragma once
// Project specific
#include <AudioHandler.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
//#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        AudioHandler::AudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
        AudioHandler::~AudioHandler() {}
        void AudioHandler::StartAudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new AudioHandler(p_sharedContext);
            }
        }

        void AudioHandler::StopAudioHandler()
        {
            delete m_singleton;
            m_singleton = nullptr;
        }

        void AudioHandler::Initialize()
        {
            m_currentFrequency = 0;
            m_continuousFrequencyAnalyserChannelID = 99999;
            m_continuousFrequencyAnalyserSoundID = 0;
            m_repeatableFrequencyAnalyserChannelID = 99999;
            m_repeatableFrequencyAnalyserSoundID = 0;
            m_outputRepeatableSoundChannelID = 99999;
            m_outputRepeatableSoundID = 0;
            m_accumulatedDeltaTime = 0;
            m_repeatableAnalysisComplete = false;
            m_frequencyVectorPrecision = 0.01;
            m_timeGunReloadButtonWasPressed = 0;
        }
        AudioHandler* AudioHandler::m_singleton = nullptr;
        AudioHandler* AudioHandler::GetInstance() { return m_singleton; }

        void AudioHandler::SetGunButtonDownTime(double p_time) { m_timeGunReloadButtonWasPressed = p_time; }

        void AudioHandler::SetupContinuousRecording()
        {
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            m_continuousFrequencyAnalyserSoundID = t_audioModule.SetupRecording(true);
        }
        void AudioHandler::SetupRepeatableRecording()
        {
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            m_repeatableFrequencyAnalyserSoundID = t_audioModule.SetupRecording(false);
        }
        void AudioHandler::StartContinuousRecording()
        {
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            t_audioModule.StopRecording();
            t_audioModule.StartRecording(m_continuousFrequencyAnalyserSoundID, true);
            m_SoundState = HOLDCONTINUOUSANALYSIS;
        }
        void AudioHandler::StartRepeatableRecording()
        {
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            t_audioModule.StopRecording();
            m_currentFrequency = 0;
            t_audioModule.StartRecording(m_repeatableFrequencyAnalyserSoundID, false); /**TODOLH Behövs stop å start recording?*/
            t_audioModule.SetVolumeOnChannel(m_continuousFrequencyAnalyserChannelID,
                                             0.0f); /**TODOLH Detta setvolume på recordkanalernas playback känns inte optimalt. Revisit maybe*/
            m_SoundState = HOLDREPEATABLEANALYSIS;
            m_repeatableAnalysisComplete = false;
        }

        float AudioHandler::GetRepeatableSoundFrequency()
        {
            if(m_repeatableAnalysisComplete)
            {
                DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
                double t_timeElapsed = t_audioModule.GetSoundTimePointer(m_outputRepeatableSoundChannelID);
                double t_soundLength = t_audioModule.GetSoundLength(m_outputRepeatableSoundID);
                double t_percentualPosition = t_timeElapsed / t_soundLength;
                double t_size = m_frequencies.size() - 1;

                int arrayPosition = (int)t_size * t_percentualPosition;

                float retValue = m_frequencies[arrayPosition];
                return retValue;
            }
            else
            {
                return 0;
            }
        }

        void AudioHandler::PlayRepeatableRecordedSound()
        {
            if (m_outputRepeatableSoundID < 1000)
            {
                DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
                t_audioModule.PlayASound(m_outputRepeatableSoundID, false, m_outputRepeatableSoundChannelID);
            }
        }
        void AudioHandler::Update(double p_deltaTime)
        {

            unsigned int recordPointer;
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            switch(m_SoundState)
            {
                case Doremi::Core::AudioHandler::ANALYSECONTINUOUS:
                    m_currentFrequency = t_audioModule.AnalyseSoundSpectrum(m_continuousFrequencyAnalyserChannelID);
                    break;

                case Doremi::Core::AudioHandler::HOLDCONTINUOUSANALYSIS:
                    recordPointer = t_audioModule.GetRecordPointer();
                    if(recordPointer > 9600)
                    {
                        m_SoundState = ANALYSECONTINUOUS;
                        // Fulfix för att kunna återanvända kanalen 99999 är min -1. Default värde som inte kan användas. I komponenterna sätts det i
                        // defaultkontruktorn.
                        if(m_continuousFrequencyAnalyserChannelID != 99999)
                        {
                            /**
                            TODOLH Behövs nog uppdatera ljudposition till listener position.
                            */
                            t_audioModule.PlaySoundOnSpecificChannel(m_continuousFrequencyAnalyserSoundID, true, m_continuousFrequencyAnalyserChannelID);
                        }
                        else
                        {
                            t_audioModule.PlayASound(m_continuousFrequencyAnalyserSoundID, true, m_continuousFrequencyAnalyserChannelID);
                        }
                        t_audioModule.SetPriority(m_continuousFrequencyAnalyserChannelID, 0);
                        /** TODOLH osäker på om detta behövs.
                        Tydligen kan det bli så att fmod snor kanaler. Men då tar den kanaler som har låg prio*/
                    }
                    break;

                case Doremi::Core::AudioHandler::ANALYSEREPEATABLE:
                    recordPointer = t_audioModule.GetRecordPointer();
                    /**
                    TODOLH Här ska vi koda in arrayen som spar ner analysdatan
                    */
                    // Kolla om ljudet är "klart" isåfall övergå till andra inspelingssättet.
                    if(!t_audioModule.IsRecording())
                    {
                        m_SoundState = HOLDCONTINUOUSANALYSIS;
                        m_repeatableAnalysisComplete = true;
                        t_audioModule.StopRecording();
                        t_audioModule.StartRecording(m_continuousFrequencyAnalyserSoundID, true);
                        t_audioModule.SetVolumeOnChannel(m_continuousFrequencyAnalyserChannelID, 1.0f);
                        m_outputRepeatableSoundID = t_audioModule.TestCopy(m_repeatableFrequencyAnalyserSoundID, m_timeGunReloadButtonWasPressed);
                        t_audioModule.PlayASound(m_outputRepeatableSoundID, false, m_outputRepeatableSoundChannelID);
                        t_audioModule.SetVolumeOnChannel(m_outputRepeatableSoundChannelID, 0);
                    }
                    else
                    {
                        // Fixa in i array för att användas med beamen senare!
                        m_accumulatedDeltaTime += p_deltaTime;
                        if(m_accumulatedDeltaTime > m_frequencyVectorPrecision)
                        {
                            m_accumulatedDeltaTime = 0; /// Ta tidsintervallet multiplicera med m_frequencyVectorPrecision å ta det arrayvärdet!!
                            m_frequencies.push_back(t_audioModule.AnalyseSoundSpectrum(m_repeatableFrequencyAnalyserChannelID));
                        }
                        else
                        {
                            // do nothing
                        }
                    }
                    break;
                case Doremi::Core::AudioHandler::HOLDREPEATABLEANALYSIS:
                    recordPointer = t_audioModule.GetRecordPointer();
                    if(recordPointer > 9600)
                    {
                        m_SoundState = ANALYSEREPEATABLE;
                        m_frequencies.clear();
                        t_audioModule.PlayASound(m_repeatableFrequencyAnalyserSoundID, false, m_repeatableFrequencyAnalyserChannelID);
                        t_audioModule.SetVolumeOnChannel(m_repeatableFrequencyAnalyserChannelID, 0.0f);
                        t_audioModule.SetPriority(m_repeatableFrequencyAnalyserChannelID, 1);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
