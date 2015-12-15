#include <Internal/AudioModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>
#include <windows.h>

namespace DoremiEngine
{
    namespace Audio
    {
        AudioModuleImplementation::AudioModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        AudioModuleImplementation::~AudioModuleImplementation() {}

        void AudioModuleImplementation::ERRCHECK(const FMOD_RESULT& p_Result)
        {
            if(p_Result != FMOD_OK)
            {
                printf("FMOD error! (%d) %s\n", p_Result, FMOD_ErrorString(p_Result));
                // exit(5);
            }
        }

        void AudioModuleImplementation::Startup()
        {
            m_fmodResult = FMOD::System_Create(&m_fmodSystem);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSystem->getVersion(&m_fmodVersion);
            if(m_fmodVersion < FMOD_VERSION)
            {
                printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", m_fmodVersion, FMOD_VERSION);
                return;
            }
            m_fmodResult = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
            ERRCHECK(m_fmodResult);
            if (m_fmodResult == 0)
            {
                m_initOK = true;
            }
            else
            {
                m_initOK = false;
            }
            int numDrivers;
            m_fmodResult = m_fmodSystem->getNumDrivers(&numDrivers);
            if(m_fmodResult != 0)
            {
                // return false;
                int derp = 0;
            }

            // No sound cards (disable sound)
            if(numDrivers == 0)
            {
                m_fmodResult = m_fmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
                if(m_fmodResult != 0)
                {
                    int derp = 0;
                }
                m_fmodResult = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
                ERRCHECK(m_fmodResult);
            }
        }

        bool AudioModuleImplementation::GetInitializationStatus() { return m_initOK; }

        void AudioModuleImplementation::Shutdown()
        {
            size_t t_soundBufferLength = m_fmodSoundBuffer.size();
            for(size_t i = 0; i < t_soundBufferLength; i++)
            {
                m_fmodResult = m_fmodSoundBuffer[i]->release();
                ERRCHECK(m_fmodResult);
            }
            m_fmodResult = m_fmodSystem->close();
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSystem->release();
            ERRCHECK(m_fmodResult);
        }

        size_t AudioModuleImplementation::LoadSound(const std::string& p_soundName, float p_minDistance, float p_maxDistance)
        {
            std::string fileLocation = m_sharedContext.GetWorkingDirectory() + p_soundName;
            std::string t_soundName = p_soundName;
            char* t_name = new char;
            for(size_t i = 0; i < m_fmodSoundBuffer.size(); i++)
            {
                m_fmodSoundBuffer[i]->getName(t_name, 250);
                std::string t_string;
                t_string = std::string(t_name);
                size_t derp = t_string.size();
                size_t hej = t_soundName.find_last_of("/", t_string.size());
                t_soundName.erase(0, hej + 1);
                if(t_name == t_soundName)
                {
                    delete t_name;
                    return i;
                }
            }
            FMOD::Sound* t_fmodSound;
            m_fmodResult = m_fmodSystem->createSound(fileLocation.c_str(), FMOD_3D, 0, &t_fmodSound);
            ERRCHECK(m_fmodResult);
            m_fmodResult = t_fmodSound->set3DMinMaxDistance(p_minDistance * m_distanceFactor, p_maxDistance * m_distanceFactor);
            ERRCHECK(m_fmodResult);
            m_fmodResult = t_fmodSound->setMode(FMOD_LOOP_NORMAL);
            ERRCHECK(m_fmodResult);
            m_fmodSoundBuffer.push_back(t_fmodSound);
            size_t returnVal = m_fmodSoundBuffer.size() - 1;
            return returnVal;
        }

        int AudioModuleImplementation::SetSoundPositionAndVelocity(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_velocity, const size_t& p_channelID)
        {
            FMOD_VECTOR pos = {p_position.x * m_distanceFactor, p_position.y * m_distanceFactor, p_position.z * m_distanceFactor};
            FMOD_VECTOR vel = { p_velocity.x * m_distanceFactor, p_velocity.y * m_distanceFactor, p_velocity.z * m_distanceFactor};
            m_fmodResult = m_fmodChannel[p_channelID]->set3DAttributes(&pos, &vel);
            ERRCHECK(m_fmodResult);
            return 0;
        }

        int AudioModuleImplementation::SetListenerPos(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_forward, DirectX::XMFLOAT3 p_up)
        {
            FMOD_VECTOR forward = {p_forward.x, p_forward.y, p_forward.z};
            FMOD_VECTOR up = {p_up.x, p_up.y, p_up.z};
            FMOD_VECTOR listenerPos = {p_position.x, p_position.y, p_position.z};

            m_fmodResult = m_fmodSystem->set3DListenerAttributes(0, &listenerPos, 0, &forward, &up);
            ERRCHECK(m_fmodResult);
            return 0;
        }

        void AudioModuleImplementation::PlayASound(size_t p_soundID, bool p_loop, size_t& p_channelID)
        {
            if(p_loop)
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundID]->setMode(FMOD_LOOP_NORMAL);
            }
            else
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundID]->setMode(FMOD_LOOP_OFF);
            }
            size_t t_lenght = m_fmodChannel.size();
            bool t_isPlaying = false;
            for(size_t i = 0; i < t_lenght; i++)
            {
                m_fmodChannel[i]->isPlaying(&t_isPlaying);
                if(!t_isPlaying)
                {
                    p_channelID = i;
                    break;
                }
                else
                {
                    // Do nothing
                }
            }
            if(p_channelID == 99999) /**Not the best coding standards but size_t isnt defined for -1 and might bug. This value is initialized in component TODOLH*/
            {
                FMOD::Channel* t_channel = 0;
                m_fmodChannel.push_back(t_channel);
                p_channelID = t_lenght;
            }
            else
            {
                // Do nothing
            }
            m_fmodResult = m_fmodSystem->playSound(FMOD_CHANNEL_FREE, m_fmodSoundBuffer[p_soundID], false, &m_fmodChannel[p_channelID]);
            m_fmodChannel[p_channelID]->setVolume(0.5f);
        }

        bool AudioModuleImplementation::GetChannelPlaying(const size_t& p_channelID)
        {
            bool t_isPlaying = false;
            m_fmodChannel[p_channelID]->isPlaying(&t_isPlaying);
            return t_isPlaying;
        }

        int AudioModuleImplementation::SetVolumeOnChannel(const size_t& p_channelID, float p_volume)
        {
            m_fmodChannel[p_channelID]->setVolume(p_volume);
            return 0;
        }

        void AudioModuleImplementation::Update() { m_fmodSystem->update(); }

        unsigned int AudioModuleImplementation::GetRecordPointer()
        {
            unsigned int timeElapsedSinceRecordingStarted = 0;
            m_fmodSystem->getRecordPosition(0, &timeElapsedSinceRecordingStarted);
            // funkar bara om vi använder outputdriver 0 ... 0 = default by OS. kan undvikas genom att lägga in driverchoice i starten TODOLH
            return timeElapsedSinceRecordingStarted;
        }

        int AudioModuleImplementation::Setup3DSound(float p_dopplerScale, float p_distanceFactor, float p_rollOffScale)
        {
            m_fmodResult = m_fmodSystem->set3DSettings(p_dopplerScale, p_distanceFactor, p_rollOffScale);
            ERRCHECK(m_fmodResult);
            m_distanceFactor = p_distanceFactor;
            return 0;
        }

        size_t AudioModuleImplementation::SetupRecording(bool p_loop)
        {
            FMOD::Sound* t_fmodSound;
            FMOD_CREATESOUNDEXINFO exinfo;
            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.numchannels = 1;
            exinfo.format = FMOD_SOUND_FORMAT_PCM16;
            exinfo.defaultfrequency = 48000;
            exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 5;

            if(p_loop)
            {
                m_fmodResult = m_fmodSystem->createSound(0, FMOD_3D | FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &t_fmodSound);
            }
            else
            {
                m_fmodResult = m_fmodSystem->createSound(0, FMOD_3D | FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_OPENUSER, &exinfo, &t_fmodSound);
            }
            ERRCHECK(m_fmodResult);
            m_fmodSoundBuffer.push_back(t_fmodSound);
            size_t r_retVal = m_fmodSoundBuffer.size() - 1;
            return r_retVal;
        }

        int AudioModuleImplementation::StartRecording(size_t p_soundID, bool p_loopRec)
        {
            m_fmodResult = m_fmodSystem->recordStart(0, m_fmodSoundBuffer[p_soundID], p_loopRec); /**0an kan stå för driver TODOLH*/
            ERRCHECK(m_fmodResult);
            return 0;
        }

        float AudioModuleImplementation::AnalyseSoundSpectrum(const size_t& p_channelID)
        {
            FMOD_RESULT t_result;
            float spectrum[m_spectrumSize];
            t_result = m_fmodChannel[p_channelID]->getSpectrum(spectrum, m_spectrumSize, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE);
            ERRCHECK(t_result);

            float max = 0;
            size_t highestFrequencyBand = 0;
            for(size_t i = 0; i < 8192; i++)
            {
                if(spectrum[i] > 0.0001f && spectrum[i] > max)
                {
                    max = spectrum[i];
                    highestFrequencyBand = i;
                }
                else
                {
                    // Do nothing
                }
            }
            float dominantHz = (float)highestFrequencyBand * m_binSize;
            /*if (dominantHz < 20)
            {
                dominantHz = 0;
            }*/
            return dominantHz;
        }
    }
}

DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Audio::AudioModule* audio = new DoremiEngine::Audio::AudioModuleImplementation(p_sharedContext);
    return audio;
}