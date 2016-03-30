#include <Internal/AudioModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>
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
            if(m_fmodResult == 0)
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
            }

            // No sound cards (disable sound)
            if(numDrivers == 0)
            {
                m_fmodResult = m_fmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
                if(m_fmodResult != 0)
                {
                }
                m_fmodResult = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
                ERRCHECK(m_fmodResult);
            }
            m_fmodResult = m_fmodSystem->createChannelGroup(NULL, &m_musicGroup);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSystem->createChannelGroup(NULL, &m_effectGroup);
            ERRCHECK(m_fmodResult);

            // Add the custom channel groups to the master channel group
            FMOD::ChannelGroup* t_masterChannelGroup;
            m_fmodSystem->getMasterChannelGroup(&t_masterChannelGroup);
            t_masterChannelGroup->addGroup(m_musicGroup);
            t_masterChannelGroup->addGroup(m_effectGroup);


            // Get and set start volumes
            float musicVolume = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().MusicVolume;
            float masterVolume = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().MasterVolume;
            float effectVolume = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().EffectVolume;

            m_musicGroup->setVolume(musicVolume);
            m_effectGroup->setVolume(effectVolume);
            t_masterChannelGroup->setVolume(masterVolume);

            // Create channel groups
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

        void AudioModuleImplementation::SetMasterVolume(const float& p_volume)
        {
            FMOD::ChannelGroup* t_masterChannelGroup;
            m_fmodSystem->getMasterChannelGroup(&t_masterChannelGroup);
            t_masterChannelGroup->setVolume(p_volume);
        }

        double AudioModuleImplementation::GetSoundTimePointer(const int& p_channelID)
        {
            unsigned int t_elapsedTime;
            m_fmodChannel[p_channelID]->getPosition(&t_elapsedTime, FMOD_TIMEUNIT_MS);
            double returnVal = (double)t_elapsedTime / 1000.0f;
            return returnVal;
        }

        double AudioModuleImplementation::GetSoundLength(const int& p_soundID)
        {
            unsigned int t_length;
            m_fmodSoundBuffer[p_soundID]->getLength(&t_length, FMOD_TIMEUNIT_MS);
            double returnVal = (double)t_length / 1000.0f;
            return returnVal;
        }

        bool AudioModuleImplementation::IsRecording()
        {
            bool t_isRecording;
            m_fmodSystem->isRecording(0, &t_isRecording); /**TODOLH om vi byter till att inte använda default recordingdriver så måste du ändra här*/
            return t_isRecording;
        }
        int AudioModuleImplementation::CopySound(int p_soundIDToCopy, int p_soundIDDestination, float p_length)
        {
            // for every second in the buffer there is sizeof(short)*44100 bytes = 2*44100 (* numchannels)
            void* testStart;
            void* testest;
            unsigned int testLength;
            FMOD_SOUND_TYPE derp;
            FMOD_SOUND_FORMAT format;
            int chans;
            int bits;
            m_fmodResult = m_fmodSoundBuffer[p_soundIDToCopy]->getFormat(&derp, &format, &chans, &bits);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSoundBuffer[p_soundIDToCopy]->lock(0, 44100 * p_length * chans * sizeof(short), &testStart, &testest, &testLength, 0);
            ERRCHECK(m_fmodResult);
            FMOD::Sound* t_fmodSound;
            FMOD_CREATESOUNDEXINFO exinfo;
            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.numchannels = 1;
            exinfo.format = FMOD_SOUND_FORMAT_PCM16;
            exinfo.defaultfrequency = 44100;
            exinfo.length = exinfo.defaultfrequency * exinfo.numchannels * p_length;
            exinfo.length = 44100 * chans * p_length * sizeof(short);

            unsigned int testLength2;
            void* testStart2;
            m_fmodResult = m_fmodSystem->createSound(0, FMOD_3D | FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_OPENUSER, &exinfo, &t_fmodSound);
            ERRCHECK(m_fmodResult);
            m_fmodResult = t_fmodSound->lock(0, 44100 * p_length, &testStart2, 0, &testLength2, 0);
            ERRCHECK(m_fmodResult);
            signed short* testArray = (signed short*)testStart;
            signed short* testArray2 = (signed short*)testStart2;
            for(int i = 0; i < 44100 * p_length * chans; i++)
            {
                testArray2[i] = testArray[i];
            }
            m_fmodResult = t_fmodSound->unlock(testArray2, 0, testLength2, 0);
            ERRCHECK(m_fmodResult);
            m_fmodResult = m_fmodSoundBuffer[p_soundIDToCopy]->unlock(testStart, 0, 44100 * 5 * chans * sizeof(short), 0);
            unsigned int hejsan;
            m_fmodSoundBuffer[p_soundIDToCopy]->getLength(&hejsan, FMOD_TIMEUNIT_MS);
            ERRCHECK(m_fmodResult);
            int returnVal;
            /// -1 is the default value for my size_ts think of it as -1 on an int
            if(p_soundIDDestination == -1)
            {
                m_fmodSoundBuffer.push_back(t_fmodSound);
                returnVal = m_fmodSoundBuffer.size() - 1;
            }
            else
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundIDDestination]->release();
                ERRCHECK(m_fmodResult);
                m_fmodSoundBuffer[p_soundIDDestination] = t_fmodSound;
                returnVal = p_soundIDDestination;
            }
            return returnVal;
        }

        int AudioModuleImplementation::LoadSound(const std::string& p_soundName, float p_minDistance, float p_maxDistance)
        {
            std::string fileLocation = m_sharedContext.GetWorkingDirectory() + p_soundName;
            std::string t_soundName = p_soundName;

            const int maxNameLength = 250;
            char* t_name = new char[maxNameLength]();
            for(size_t i = 0; i < m_fmodSoundBuffer.size(); i++)
            {
                m_fmodSoundBuffer[i]->getName(t_name, maxNameLength);
                std::string t_string;
                t_string = std::string(t_name);
                size_t hej = t_soundName.find_last_of("/", t_string.size());
                t_soundName.erase(0, hej + 1);
                if(t_name == t_soundName)
                {
                    delete[] t_name;
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
            int returnVal = m_fmodSoundBuffer.size() - 1;
            delete[] t_name;
            return returnVal;
        }

        int AudioModuleImplementation::SetSoundPositionAndVelocity(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_velocity, const int& p_channelID)
        {
            FMOD_VECTOR pos = {p_position.x * m_distanceFactor, p_position.y * m_distanceFactor, p_position.z * m_distanceFactor};
            FMOD_VECTOR vel = {p_velocity.x * m_distanceFactor, p_velocity.y * m_distanceFactor, p_velocity.z * m_distanceFactor};
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

        void AudioModuleImplementation::PlaySoundOnSpecificChannel(const int& p_soundID, bool p_loop, const int& p_channelID, const SoundGroup& p_soundGroup)
        {
            if(p_loop)
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundID]->setMode(FMOD_LOOP_NORMAL);
            }
            else
            {
                m_fmodResult = m_fmodSoundBuffer[p_soundID]->setMode(FMOD_LOOP_OFF);
            }
            m_fmodResult = m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, m_fmodSoundBuffer[p_soundID], false, &m_fmodChannel[p_channelID]);
            SetChannelGroup(m_fmodChannel[p_channelID], p_soundGroup);
        }

        void AudioModuleImplementation::PlayASound(int p_soundID, bool p_loop, int& p_channelID, const SoundGroup& p_soundGroup)
        {
            if(p_soundID < 0 || p_soundID >= m_fmodSoundBuffer.size())
            {
                // TODOKO log error, no sound with that id
                return;
            }
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
            if(p_channelID <= -1)
            {
                FMOD::Channel* t_channel = 0;
                m_fmodChannel.push_back(t_channel);
                p_channelID = t_lenght;
            }
            else
            {
                // Do nothing
            }
            m_fmodResult = m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, m_fmodSoundBuffer[p_soundID], false, &m_fmodChannel[p_channelID]);
            SetChannelGroup(m_fmodChannel[p_channelID], p_soundGroup);
        }

        void AudioModuleImplementation::SetChannelGroup(FMOD::Channel* o_channel, SoundGroup p_group)
        {
            switch (p_group)
            {
            case DoremiEngine::Audio::SoundGroup::Music:
                o_channel->setChannelGroup(m_musicGroup);
                break;
            case DoremiEngine::Audio::SoundGroup::Effect:
                o_channel->setChannelGroup(m_effectGroup);
                break;
            case DoremiEngine::Audio::SoundGroup::RecordAndAnalyse:
                break;
            default:
                break;
            }
        }

        void AudioModuleImplementation::SetPriority(const int& p_channelID, const int& p_priority)
        {
            m_fmodChannel[p_channelID]->setPriority(p_priority);
        }

        bool AudioModuleImplementation::GetChannelPlaying(const int& p_channelID)
        {
            bool t_isPlaying = false;
            m_fmodChannel[p_channelID]->isPlaying(&t_isPlaying);
            return t_isPlaying;
        }

        int AudioModuleImplementation::SetVolumeOnChannel(const int& p_channelID, float p_volume)
        {
            if(p_channelID < 0 || p_channelID >= m_fmodSoundBuffer.size())
            {
                return 0;
            }
            m_fmodChannel[p_channelID]->setVolume(p_volume);
            return 0;
        }

        void AudioModuleImplementation::Update() { m_fmodSystem->update(); }

        unsigned int AudioModuleImplementation::GetRecordPointer()
        {
            unsigned int timeElapsedSinceRecordingStarted = 0;
            m_fmodResult = m_fmodSystem->getRecordPosition(0, &timeElapsedSinceRecordingStarted);
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

        int AudioModuleImplementation::SetupRecording(bool p_loop)
        {
            FMOD::Sound* t_fmodSound;
            FMOD_CREATESOUNDEXINFO exinfo;
            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.numchannels = 1;
            exinfo.format = FMOD_SOUND_FORMAT_PCM16;
            exinfo.defaultfrequency = 44100;
            exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 5;

            if(p_loop)
            {
                m_fmodResult = m_fmodSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &t_fmodSound);
            }
            else
            {
                m_fmodResult = m_fmodSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_OPENUSER, &exinfo, &t_fmodSound);
            }
            ERRCHECK(m_fmodResult);
            m_fmodSoundBuffer.push_back(t_fmodSound);
            int r_retVal = m_fmodSoundBuffer.size() - 1;
            return r_retVal;
        }

        int AudioModuleImplementation::StartRecording(int p_soundID, bool p_loopRec)
        {
            m_fmodResult = m_fmodSystem->recordStart(0, m_fmodSoundBuffer[p_soundID], p_loopRec); /**0an kan stå för driver TODOLH*/
            ERRCHECK(m_fmodResult);
            return 0;
        }

        void AudioModuleImplementation::StopRecording()
        {
            m_fmodResult = m_fmodSystem->recordStop(0);
            ERRCHECK(m_fmodResult);
        }
        float AudioModuleImplementation::AnalyseSoundSpectrum(const int& p_channelID)
        {
            FMOD_RESULT t_result;
            float spectrum[m_spectrumSize];
            t_result = m_fmodChannel[p_channelID]->getSpectrum(spectrum, m_spectrumSize, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE);
            ERRCHECK(t_result);
            float amplitudes[m_spectrumSize];
            t_result = m_fmodChannel[p_channelID]->getWaveData(amplitudes, m_spectrumSize, 0);
            ERRCHECK(t_result);

            float max = 0;
            size_t highestFrequencyBand = 0;
            for(size_t i = 0; i < m_spectrumSize; i++)
            {
                if(spectrum[i] > 0.0001f && spectrum[i] > max && abs(amplitudes[i]) > m_cutOffAmplitude)
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

            if(dominantHz < 20)
            {
                dominantHz = 0;
            }
            return dominantHz;
        }

        void AudioModuleImplementation::StopSound(const int& p_channelID)
        {
            if(p_channelID >= 0 && p_channelID < m_fmodChannel.size())
            {
                m_fmodChannel[p_channelID]->stop();
            }
            else
            {
                std::cout << "Error when stopping sound, no such channel" << std::endl;
            }
        }

        void AudioModuleImplementation::SetSoundGroupVolume(const float& p_volume, const SoundGroup& p_group)
        {
            switch(p_group)
            {
                case DoremiEngine::Audio::SoundGroup::Music:
                    m_musicGroup->setVolume(p_volume);
                    break;
                case DoremiEngine::Audio::SoundGroup::Effect:
                    m_effectGroup->setVolume(p_volume);
                    break;
                case DoremiEngine::Audio::SoundGroup::RecordAndAnalyse:
                    break;
                default:
                    break;
            }
        }

        void AudioModuleImplementation::StopSoundGroup(const SoundGroup& p_group)
        {
            switch(p_group)
            {
                case DoremiEngine::Audio::SoundGroup::Music:
                    m_musicGroup->stop();
                    break;
                case DoremiEngine::Audio::SoundGroup::Effect:
                    m_effectGroup->stop();
                    break;
                case DoremiEngine::Audio::SoundGroup::RecordAndAnalyse:
                    break;
                default:
                    break;
            }
        }
    }
}

DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Audio::AudioModule* audio = new DoremiEngine::Audio::AudioModuleImplementation(p_sharedContext);
    return audio;
}