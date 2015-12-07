#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <vector>
#if defined(_WINDLL)
#define INPUT_DLL_EXPORT __declspec(dllexport)
#else
#define INPUT_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Input
    {
        /**
        TODO docs
        */
        class InputModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            TODO docs
            */
            virtual void Startup() = 0;

            /**
            TODO docs
            */
            virtual void SetWorkingDirectory(const std::string& p_workingDirectory) = 0;

            /**
            TODO docs
            */
            virtual void Shutdown() = 0;

            virtual void Update() = 0;
            virtual int CreateWindow(int p_width, int p_height) = 0;
            virtual void PrintInputStructsDEBUG() = 0;
            virtual void PrintInputMouseMovement() = 0;

            /////GET//////////////////
            virtual const std::vector<int> GetKeyBoardInput() const = 0;
            virtual const std::vector<int> GetMouseButtonInput() const = 0;
            virtual const int GetMouseMovementX() const = 0;
            virtual const int GetMouseMovementY() const = 0;
            //////////////////////////
        };
    }
}

extern "C" {
    typedef DoremiEngine::Input::InputModule* (*CREATE_INPUT_MODULE)(const DoremiEngine::Core::SharedContext&);
    INPUT_DLL_EXPORT DoremiEngine::Input::InputModule*
        CreateInputModule(const DoremiEngine::Core::SharedContext& p_context);
}