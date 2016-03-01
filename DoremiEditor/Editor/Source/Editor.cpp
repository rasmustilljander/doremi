// Project specific
#include <Editor.hpp>

// Engine
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>


// Game
#include <Doremi/Core/Include/GameCore.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>

// Timer
#include <Doremi/Core/Include/Timing/TimerManager.hpp>


// Standard libraries
#include <stdexcept>
#include <exception>
#include <chrono>
#include <iostream>

using namespace std;

namespace DoremiEditor
{
    using namespace Doremi::Core;

    EditorMain::EditorMain() {}

    EditorMain::~EditorMain() {}

    void EditorMain::Initialize()
    {
        TIME_FUNCTION_START
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(DoremiEngine::Core::EngineModuleEnum::NETWORK);
        m_mayaLoader = new MayaLoader(1024, 768); // Hard coded resolution just to get things to work. TODOCONFIG
        TIME_FUNCTION_STOP
    }

    void EditorMain::Run()
    {
        TimeHandler* t_timeHandler = TimeHandler::GetInstance();

        t_timeHandler->PreviousClock = std::chrono::high_resolution_clock::now();

        while(true)
        {
            // Tick time
            t_timeHandler->Tick();

            // Loop as many update-steps we will take this frame
            while(t_timeHandler->ShouldUpdateFrame())
            {
                // Update Game logic
                Update(t_timeHandler->UpdateStepLen);

                // Update accumulator and gametime
                t_timeHandler->UpdateAccumulatorAndGameTime();
            }
        }
        TIME_FUNCTION_STOP
    }

    void EditorMain::Update(double p_deltaTime)
    {
        TIME_FUNCTION_START

        cout << "Update" << endl;
        m_mayaLoader->TryReadAMessage();
        // Render things
        TIME_FUNCTION_STOP
    }

    void EditorMain::Start()
    {
        TIME_FUNCTION_START
        Initialize();
        Run();
        TIME_FUNCTION_STOP
    }
}