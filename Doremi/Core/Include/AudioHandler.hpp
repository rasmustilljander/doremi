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
            static void StartAudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static void StopAudioHandler();
            AudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~AudioHandler();
            // Initializes the handler. Mostly sets values to 0 or channelID to 99999 (used as a "error" value)
            void Initialize();
            // Sets the variable for how long the reloadbutton was pressed and therefore how long the sound should be
            void SetGunButtonDownTime(double p_time);
            // Starts the recording that streams open mic
            void StartContinuousRecording();
            // Starts the recording that records while the button is pressed to be able to replay that sound
            void StartRepeatableRecording();
            // Sets up the streaming open mic recoreder
            void SetupContinuousRecording();
            // Sets up the repeatable recorder
            void SetupRepeatableRecording();
            // Plays the sound that we recorded
            void PlayRepeatableRecordedSound();
            // Update to analyse sound. Contains switch case to control what sound is supposed to be analysed and when.
            void Update(double p_deltaTime);
            float GetFrequency() const { return m_currentFrequency; };
            // Returns the current frequency from the repeatable sound. It is fetched from an array with the help of the recordpointer from the
            // playing sound.
            float GetRepeatableSoundFrequency();

        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            InputHandler* m_inputHandler;

            double m_accumulatedDeltaTime;
            // Vector of the frequencies from the repeatable sound
            std::vector<float> m_frequencies;

            float m_frequencyVectorPrecision;
            static AudioHandler* m_singleton;
            // Sound IDs and ChannelIDs
            size_t m_continuousFrequencyAnalyserChannelID;
            size_t m_continuousFrequencyAnalyserSoundID;
            size_t m_repeatableFrequencyAnalyserChannelID;
            size_t m_repeatableFrequencyAnalyserSoundID;
            size_t m_outputRepeatableSoundChannelID;
            size_t m_outputRepeatableSoundID;
            // The current frequency that just got analysed
            float m_currentFrequency;
            // Used to control the flow of pdate function. Cant use the function until the array is filled with values
            bool m_repeatableAnalysisComplete;
            // Keeps the time that the gunreloadbutton was pressed down
            double m_timeGunReloadButtonWasPressed;

            // Used to control the flow of the update function
            enum SoundState
            {
                ANALYSECONTINUOUS,
                HOLDCONTINUOUSANALYSIS,
                ANALYSEREPEATABLE,
                HOLDREPEATABLEANALYSIS,

            };
            SoundState m_SoundState;
        };
    }
}
