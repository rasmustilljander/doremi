#include <Doremi/Core/Include/TimeHandler.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        TimeHandler* TimeHandler::m_singleton = nullptr;

        TimeHandler* TimeHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new TimeHandler();
            }
            return m_singleton;
        }

        void TimeHandler::Tick()
        {
            CurrentClock = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = (CurrentClock - PreviousClock);
            Frame = duration.count() + Offset;
            Offset = 0;

            // We simulate maximum 250 milliseconds each frame
            // If we would let it be alone we would get mayor stops instead of lesser ones that will slowly catch up
            if(Frame > MaxFrameTime)
            {
                Offset = Frame - MaxFrameTime;
                Frame = MaxFrameTime;
                std::cout << "Frame took more then " << MaxFrameTime << " Seconds" << std::endl;
            }

            // Update the previous position with frametime so we can catch up if we slow down
            PreviousClock = CurrentClock;

            // Update Accumulator (how much we will work this frame)
            Accum += Frame;
        }

        void TimeHandler::UpdateAccumulatorAndGameTime()
        {
            // Remove time from accumulator
            // Accumulator -= UpdateTimeStepLength;
            Accum -= UpdateStepLen;

            // Add time to start
            GameTime += UpdateStepLen;
        }

        double TimeHandler::GetFrameAlpha() { return Accum / UpdateStepLen; }

        bool TimeHandler::ShouldUpdateFrame() { return Accum >= UpdateStepLen; }

        TimeHandler::TimeHandler() {}

        TimeHandler::~TimeHandler() {}
    }
}