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

        float InputModuleImplementation::DoFunction(float a, float b)
        {
            using namespace std;
            cout << "From .dll" << std::endl;
            cout << "a" << a << std::endl;
            cout << "b" << b << std::endl;

            return a * b;
        }
    }
}

DoremiEngine::Input::InputModule* CreateInputModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Input::InputModule* input = new DoremiEngine::Input::InputModuleImplementation();
    return input;
}