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
            m_singleton = new AudioHandler(p_sharedContext);
        }
        void AudioHandler::Initialize()
        {
            m_currentFrequency = 0;
            m_frequencyAnalyserChannelID = 0;
            m_frequencyAnalyserSoundID = 0;
            m_analyseActive = false;
        }
        AudioHandler* AudioHandler::m_singleton = nullptr;
        AudioHandler* AudioHandler::GetInstance() { return m_singleton; }
        void AudioHandler::SetFrequencyAnalyserSoundID(const size_t& p_soundID) { m_frequencyAnalyserSoundID = p_soundID; }
        void AudioHandler::SetLoopForFrequencyAnalyser(bool p_loop) { m_loopFrequencyAnalyser = p_loop; }
        void AudioHandler::Update()
        {
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            if(m_analyseActive)
            {
                m_currentFrequency = t_audioModule.AnalyseSoundSpectrum(m_frequencyAnalyserChannelID);
            }
            else
            {
                unsigned int recordPointer = t_audioModule.GetRecordPointer();
                if(recordPointer > 9600)
                {
                    m_analyseActive = true;
                    t_audioModule.PlayASound(m_frequencyAnalyserSoundID, m_loopFrequencyAnalyser, m_frequencyAnalyserChannelID);
                }
            }
        }
    }
}
