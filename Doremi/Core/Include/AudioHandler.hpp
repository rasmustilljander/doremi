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
            void SetContinuousFrequencyAnalyserSoundID(const size_t& p_soundID);
            void SetRepeatableFrequencyAnalyserSoundID(const size_t& p_soundID);
            void StartContinuousRecording();
            void StartRepeatableRecording();
            void SetupContinuousRecording();
            void SetupRepeatableRecording();
            void PlayRepeatableRecordedSound();
            void Update(double p_deltaTime);
            float GetFrequency() const { return m_currentFrequency; };
            float GetRepeatableSoundFrequency();

            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            InputHandler* m_inputHandler;

            double m_accumulatedDeltaTime;
            std::vector<float>m_frequencies;

            float m_frequencyVectorPrecision;
            static AudioHandler* m_singleton;
            size_t m_continuousFrequencyAnalyserChannelID;
            size_t m_continuousFrequencyAnalyserSoundID;
            size_t m_repeatableFrequencyAnalyserChannelID;
            size_t m_repeatableFrequencyAnalyserSoundID;
            size_t m_outputRepeatableSoundChannelID;
            size_t m_outputRepeatableSoundID;
            float m_currentFrequency;
            bool m_repeatableAnalysisComplete;
            bool m_analyseActive;
            bool m_repeatableAnalysisActive;
            bool m_continuousRecording;
        };
    }
}
