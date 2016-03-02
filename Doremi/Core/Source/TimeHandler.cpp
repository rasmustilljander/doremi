#include "..\Include\TimeHandler.hpp"
#include "..\Include\TimeHandler.hpp"
#include <Doremi/Core/Include/TimeHandler.hpp>
#include <iostream>
#include <thread>

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
            Frame = duration.count() + LaggTime;
            LaggTime = 0.0f;

            // We simulate maximum 250 milliseconds each frame
            // If we would let it be alone we would get mayor stops instead of lesser ones that will slowly catch up
            if(Frame > MaxFrameTime)
            {
                LaggTime = Frame - MaxFrameTime;
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

        bool TimeHandler::IsLagging()
        {
            // If we have any lagg time or the frame took more then one update, we are lagging
            // TODOXX if we're moving at exaclty 60 fps, this might not work too well
            // So we keep it at 3 times for now.. that like 20 fps
            if(LaggTime > 0.0f || Frame > UpdateStepLen * 3.0f)
            {
                return false;
            }
            return false;
        }

        void TimeHandler::ResetTime()
        {
            CurrentClock = std::chrono::high_resolution_clock::now();

            // Update the previous position with frametime so we can catch up if we slow down
            PreviousClock = CurrentClock;
        }

        bool TimeHandler::FrameLessThenTimeStep() { return Frame < UpdateStepLen; }

        void TimeHandler::SleepTillNextUpdate()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>((UpdateStepLen - Frame) * 1000.0f)));
        }

        TimeHandler::TimeHandler() {}

        TimeHandler::~TimeHandler() {}
    }
}