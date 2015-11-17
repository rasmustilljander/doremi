#include <Internal/AudioModuleImplementation.hpp>
#include <Do-Re-Mi-Engine/Core/Include/SharedContext.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Audio
    {
        AudioModuleImplementation::AudioModuleImplementation()
        {

        }

        AudioModuleImplementation::~AudioModuleImplementation()
        {

        }

        void AudioModuleImplementation::Startup()
        {

        }
        
        void AudioModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
        {

        }

        void AudioModuleImplementation::Shutdown()
        {

        }

        float AudioModuleImplementation::DoFunction(float a, float b)
        {
            using namespace std;
            cout << "From .dll" << std::endl;
            cout << "a" << a << std::endl;
            cout << "b" << b << std::endl;

            return a*b;
        }
    }
}

DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Audio::AudioModule* audio = new DoremiEngine::Audio::AudioModuleImplementation();
    return audio;
}