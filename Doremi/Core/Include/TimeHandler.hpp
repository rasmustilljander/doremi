#pragma once
#include <chrono>

namespace Doremi
{
    namespace Core
    {
        class TimeHandler
        {
        public:
            static TimeHandler* GetInstance();

            double Frame = 0;
            double LaggTime = 0;
            double Accum = 0;
            double GameTime = 0;
            double UpdateStepLen = 0.017f;
            double MaxFrameTime = 0.25f;

            std::chrono::time_point<std::chrono::high_resolution_clock> CurrentClock;
            std::chrono::time_point<std::chrono::high_resolution_clock> PreviousClock;

            void Tick();

            void UpdateAccumulatorAndGameTime();

            double GetFrameAlpha();

            bool ShouldUpdateFrame();

            bool IsLagging();

            void ResetTime();

            bool FrameLessThenTimeStep();

            void SleepTillNextUpdate();

        private:
            TimeHandler();
            ~TimeHandler();

            static TimeHandler* m_singleton;
        };
    }
}
