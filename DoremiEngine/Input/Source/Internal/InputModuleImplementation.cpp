#include <Internal/InputModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Input
    {
        InputModuleImplementation::InputModuleImplementation()
        {
        }

        InputModuleImplementation::~InputModuleImplementation()
        {
        }

        void InputModuleImplementation::Startup()
        {
        }

        void InputModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
        {
        }

        void InputModuleImplementation::Shutdown()
        {
        }

        void InputModuleImplementation::Update()
        {

        }
        int InputModuleImplementation::CreateWindow(int p_width, int p_height)
        {
            return 0;
        }
        void InputModuleImplementation::PrintInputStructsDEBUG()
        {
        }
        void InputModuleImplementation::PrintInputMouseMovement()
        {
        }
        void InputModuleImplementation::SwitchCaseEventsForPlaying(SDL_Event & p_eventVariable)
        {
        }
        void InputModuleImplementation::SwitchCaseEventsForTyping(SDL_Event & p_eventVariable)
        {
        }
        void InputModuleImplementation::ResetMouseMovementStruct()
        {
        }
        void InputModuleImplementation::RemoveFromList(SDL_Keycode p_eventvariable, std::vector<int>& o_listToUse)
        {
        }
        void InputModuleImplementation::AddToList(SDL_Keycode p_eventvariable, std::vector<int>& o_listToUse)
        {
        }
    }
}

DoremiEngine::Input::InputModule* CreateInputModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Input::InputModule* input = new DoremiEngine::Input::InputModuleImplementation();
    return input;
}