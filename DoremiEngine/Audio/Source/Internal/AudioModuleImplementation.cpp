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

        void AudioModuleImplementation::Startup()
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

            return a * b;
        }
    }
}

DoremiEngine::Audio::AudioModule* CreateAudioModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Audio::AudioModule* audio =
    new DoremiEngine::Audio::AudioModuleImplementation(p_sharedContext);
    return audio;
}