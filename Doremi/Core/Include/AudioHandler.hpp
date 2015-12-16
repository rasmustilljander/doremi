#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>


namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {

        class InputHandler;
        class AudioHandler
        {
            public:
            /** Is a singleton. Use this method to get the EventManager*/
            static AudioHandler* GetInstance();
            AudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~AudioHandler();
            static void StartAudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            void Initialize();
            void SetFrequencyAnalyserSoundID(const size_t& p_soundID);
            void SetLoopForFrequencyAnalyser(bool p_loop);
            void Update();
            float GetFrequency() const { return m_currentFrequency; };

            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            InputHandler* m_inputHandler;

            static AudioHandler* m_singleton;
            size_t m_frequencyAnalyserChannelID;
            size_t m_frequencyAnalyserSoundID;
            bool m_loopFrequencyAnalyser;
            float m_currentFrequency;
            bool m_analyseActive;
        };
    }
}
