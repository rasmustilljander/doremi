// Project specific
#include <Game.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>


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
            throw std::runtime_error("1Failed to load engine - please check your installation.");
        }

        INITIALIZE_ENGINE libInitializeEngine =
        (INITIALIZE_ENGINE)DynamicLoader::LoadProcess(m_engineModule, "InitializeEngine");

        if(libInitializeEngine == nullptr)
        {
            throw std::runtime_error("2Failed to load engine - please check your installation.");
        }
        const DoremiEngine::Core::SharedContext& a =
        libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::AUDIO);
    }
}