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
            m_fmodResult = FMOD::System_Create(&m_system);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_system->getVersion(&m_fmodVersion);
            if (m_fmodVersion < FMOD_VERSION)
            {
                printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", m_fmodVersion, FMOD_VERSION);
                return;
            }
            m_fmodResult = m_system->init(32, FMOD_INIT_NORMAL, 0);
            ERRCHECK(m_fmodResult);
        }

        void AudioModuleImplementation::Shutdown()
        {
            m_fmodResult = m_sound->release();
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_system->close();
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_system->release();
            ERRCHECK(m_fmodResult);

        }

        float AudioModuleImplementation::DoFunction(float a, float b)
        {
            using namespace std;
            cout << "From .dll" << std::endl;
            cout << "a" << a << std::endl;
            cout << "b" << b << std::endl;

            return a * b;
        }

        void AudioModuleImplementation::LoadSound(const std::string& p_soundName)
        {
            //p_soundName.c_str()
            m_fmodResult = m_system->createSound("C:/build/build/x86/Debug/Sounds/Latino.wav", FMOD_HARDWARE, 0, &m_sound);
            ERRCHECK(m_fmodResult);

            m_fmodResult = m_sound->setMode(FMOD_LOOP_OFF);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_system->playSound(FMOD_CHANNEL_FREE, m_sound, false, &m_channel);
        }

        int AudioModuleImplementation::Update()
        {
            m_system->update();
            return 0;
        }
    }
}

DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Audio::AudioModule* audio =
    new DoremiEngine::Audio::AudioModuleImplementation(p_sharedContext);
    return audio;
}