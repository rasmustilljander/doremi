// Project specific
#include <Game.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/GameCore.hpp>

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

        // Start the game
        Core::GameCore* gameCore = new Core::GameCore();
        gameCore->Initialize();
        gameCore->StartCore();
    }
}