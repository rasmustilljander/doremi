// Project specific
#include <Doremi/Core/Include/MenuClasses/MenuHandler.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EventHandler/EventHandler.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        MenuHandler::MenuHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        MenuHandler::~MenuHandler() {}

        void MenuHandler::Initialize() {}

        void MenuHandler::Update(double p_dt) {}
    }
}