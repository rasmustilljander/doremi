// Project specific
#include <Editor.hpp>

// Engine
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>


// Game
#include <Doremi/Core/Include/GameCore.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>


// Managers
#include <Doremi/Core/Include/Network/NetworkManagerMaster.hpp>


// Standard libraries
#include <stdexcept>
#include <exception>
#include <chrono>
#include <iostream>


namespace DoremiEditor
{
    using namespace Doremi::Core;

    EditorMain::EditorMain() {}

    EditorMain::~EditorMain() {}

    void EditorMain::Initialize()
    {
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(DoremiEngine::Core::EngineModuleEnum::NETWORK);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/


        Manager* t_masterNetworkManager = new NetworkManagerMaster(sharedContext);

        // Add manager to list of managers
        // Remember to put server last (cause we want on same frame as we update to send data, or at least close togeather)

        m_managers.push_back(t_masterNetworkManager);
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
    }

    void EditorMain::Update(double p_deltaTime)
    {
        // Have all managers update
        size_t length = m_managers.size();
        for(size_t i = 0; i < length; i++)
        {
            m_managers.at(i)->Update(p_deltaTime);
        }
    }

    void EditorMain::Start()
    {
        Initialize();
        Run();
    }
}