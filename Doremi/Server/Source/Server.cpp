// Project specific
#include <Server.hpp>
#include <Doremi/Core/Include/GameCore.hpp>

// Third party

// Standard libraries
#include <stdexcept>
#include <exception>

namespace Doremi
{
    ServerMain::ServerMain() {}

    ServerMain::~ServerMain() {}

    void ServerMain::Start()
    {
        // Start the game
        Core::GameCore* gameCore = new Core::GameCore();
        gameCore->InitializeServer();
        gameCore->StartServerCore();
    }
}