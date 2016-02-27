// Project specific
#include <Master.hpp>

// Engine
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>


// Game
#include <Doremi/Core/Include/GameCore.hpp>


// Managers
#include <Doremi/Core/Include/Network/NetworkManagerServer.hpp>


// Timer
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>


// Standard libraries
#include <stdexcept>
#include <exception>
#include <chrono>
#include <iostream>


namespace Doremi
{
    MasterMain::MasterMain() {}

    MasterMain::~MasterMain() {}

    void MasterMain::Initialize()
    {
        TIME_FUNCTION_START
            const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(
                DoremiEngine::Core::EngineModuleEnum::NETWORK);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/


        Core::Manager* t_masterNetworkManager = new Core::NetworkManagerServer(sharedContext);

        // Add manager to list of managers
        // Remember to put server last (cause we want on same frame as we update to send data, or at least close togeather)

        m_managers.push_back(t_masterNetworkManager);

        TIME_FUNCTION_STOP
    }

    void MasterMain::Run()
    {
        TIME_FUNCTION_START
        std::chrono::time_point<std::chrono::high_resolution_clock> CurrentClock, PreviousClock;
        PreviousClock = std::chrono::high_resolution_clock::now();

        double Frame = 0;
        double Offset = 0;
        double Accum = 0;
        double GameTime = 0;
        double UpdateStepLen = 0.017 * 2; // Limit to 30 fps for debug purposes TODOJB Change back to 60 fps
        double MaxFrameTime = 0.25;

        while (true)
        {
            CurrentClock = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = (CurrentClock - PreviousClock);
            Frame = duration.count() + Offset;
            Offset = 0;


            // We simulate maximum 250 milliseconds each frame
            // If we would let it be alone we would get mayor stops instead of lesser ones that will slowly catch up
            if (Frame > MaxFrameTime)
            {
                Offset = Frame - MaxFrameTime;
                Frame = MaxFrameTime;
                std::cout << "Frame took more then " << MaxFrameTime << " Seconds" << std::endl;
            }

            // Update the previous position with frametime so we can catch up if we slow down
            PreviousClock = CurrentClock;

            // Update Accumulator (how much we will work this frame)
            Accum += Frame;

            // Loop as many update-steps we will take this frame
            while (Accum >= UpdateStepLen)
            {
                // Update Game logic
                Update(UpdateStepLen);

                // Remove time from accumulator
                Accum -= UpdateStepLen;

                // Add time to start
                GameTime += UpdateStepLen;
            }
        }
        TIME_FUNCTION_STOP
    }

    void MasterMain::Update(double p_deltaTime)
    {
        TIME_FUNCTION_START

        // Have all managers update
        size_t length = m_managers.size();
        for (size_t i = 0; i < length; i++)
        {
            m_managers.at(i)->Update(p_deltaTime);
        }

        TIME_FUNCTION_STOP
    }

    void MasterMain::Start()
    {
        TIME_FUNCTION_START
        Initialize();
        Run();
        TIME_FUNCTION_STOP
    }
}