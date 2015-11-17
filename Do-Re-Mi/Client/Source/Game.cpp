// Project specific
#include <Game.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <Do-Re-Mi-Engine/Core/Include/DoremiEngine.hpp>

// Third party

// Standard libraries
#include <stdexcept>
#include <exception>

namespace Doremi
{
    GameMain::GameMain()
    {
    }

    GameMain::~GameMain()
    {
    }

    void GameMain::Start()
    {
        m_engineModule = DynamicLoader::LoadSharedLibrary("EngineCore.dll");

        if(m_engineModule == nullptr)
        {
            throw std::runtime_error("Failed to load engine - please check your installation.");
        }

        INITIALIZE_ENGINE libInitializeEngine =
        (INITIALIZE_ENGINE)DynamicLoader::LoadProcess(m_engineModule, "InitializeEngine");

        if(libInitializeEngine == nullptr)
        {
            throw std::runtime_error("Failed to load engine - please check your installation.");
        }
        //  = libInitializeEngine(RootEngine::SubsystemInit::INIT_ALL);
    }
}