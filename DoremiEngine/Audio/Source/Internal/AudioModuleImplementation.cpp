#include <Internal/AudioModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Audio
    {
        AudioModuleImplementation::AudioModuleImplementation(const Core::SharedContext& p_sharedContext)
        : m_sharedContext(p_sharedContext)
        {
        }

        AudioModuleImplementation::~AudioModuleImplementation()
        {
        }



        void AudioModuleImplementation::ERRCHECK(const FMOD_RESULT& p_Result)
        {
            if (p_Result != FMOD_OK)
            {
                printf("FMOD error! (%d) %s\n", p_Result, FMOD_ErrorString(p_Result));
                exit(5);
            }
        }

        void AudioModuleImplementation::Startup()
        {
            m_fmodResult = FMOD::System_Create(&m_fmodSystem);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSystem->getVersion(&m_fmodVersion);
            if (m_fmodVersion < FMOD_VERSION)
            {
                printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", m_fmodVersion, FMOD_VERSION);
                return;
            }
            m_fmodResult = m_fmodSystem->init(32, FMOD_INIT_NORMAL, 0);
            ERRCHECK(m_fmodResult);
        }

        void AudioModuleImplementation::Shutdown()
        {
            size_t t_soundBufferLength = m_fmodSoundBuffer.size();
            for (size_t i = 0; i < t_soundBufferLength; i++)
            {
                m_fmodResult = m_fmodSoundBuffer[i]->release();
                ERRCHECK(m_fmodResult);
            }
            m_fmodResult = m_fmodSystem->close();
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSystem->release();
            ERRCHECK(m_fmodResult);

        }

        size_t AudioModuleImplementation::LoadSound(const std::string& p_soundName)
        {
            FMOD::Sound* t_fmodSound;
            m_fmodResult = m_fmodSystem->createSound(p_soundName.c_str(), FMOD_HARDWARE, 0, &t_fmodSound);
            ERRCHECK(m_fmodResult);

            m_fmodResult = t_fmodSound->setMode(FMOD_LOOP_NORMAL);
            ERRCHECK(m_fmodResult);
            m_fmodSoundBuffer.push_back(t_fmodSound);
            size_t returnVal = m_fmodSoundBuffer.size() - 1;
            return returnVal;
            //m_fmodResult = m_fmodSystem->playSound(FMOD_CHANNEL_FREE, t_fmodSound, false, &m_fmodChannel); C:/build/build/x86/Debug/Sounds/Latino.wav
        }

        void AudioModuleImplementation::PlaySound(size_t p_soundID, bool p_loop)
        {
            if (p_loop)
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundID]->setMode(FMOD_LOOP_NORMAL);
            }
            else
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundID]->setMode(FMOD_LOOP_OFF);
            }
            m_fmodResult = m_fmodSystem->playSound(FMOD_CHANNEL_FREE, m_fmodSoundBuffer[p_soundID], false, &m_fmodChannel);
        }

        int AudioModuleImplementation::Update()
        {
            m_fmodSystem->update();
            return 0;
        }

        size_t AudioModuleImplementation::SetupRecording()
        {
            FMOD::Sound* t_fmodSound;
            FMOD_CREATESOUNDEXINFO exinfo; 
            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.numchannels = 1;
            exinfo.format = FMOD_SOUND_FORMAT_PCM16;
            exinfo.defaultfrequency = 48000;
            exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 5;

            m_fmodResult = m_fmodSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &t_fmodSound);
            ERRCHECK(m_fmodResult);
            m_fmodSoundBuffer.push_back(t_fmodSound);
            size_t r_retVal = m_fmodSoundBuffer.size() - 1;
            return r_retVal;
        }
    }
}

DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Audio::AudioModule* audio =
    new DoremiEngine::Audio::AudioModuleImplementation(p_sharedContext);
    return audio;
}