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
        Input handler uses this class to recieve input and check if it is relevant data.
        */
        class InputModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            Initializes SDL and creates window if there isn't a window active.
            */
            virtual void Startup() = 0;

            /**
            TODO Does nothing
            */
            virtual void Shutdown() = 0;
            /**
            Resets the mousemovement struct, gets the keystrokes pressed and gets the mouseposition.
            */
            virtual void Update() = 0;
            /**
            Create a window using sdl. Returns 1 if a window is created
            */
            virtual int CreateWindowSDL(int p_width, int p_height) = 0;
            /**
                Debug, Prints out the struct recieved form sdl.
            */
            virtual void PrintInputStructsDEBUG() = 0;
            /**
                Debug, Prints out the struct recieved form sdl.
            */
            virtual void PrintInputMouseMovement() = 0;

            /**
            Get function, so the input handler can get it.
            */
            virtual const std::vector<int> GetKeyBoardInput() const = 0;
            /**
            Get function, so the input handler can get it.
            */
            virtual const std::vector<int> GetMouseButtonInput() const = 0;
            /**
            The mousemovement in x since last update, this value is reset at the start of the update in Update();
            */
            virtual int GetMouseMovementX() = 0;
            /**
            The mousemovement in y since last update, this value is reset at the start of the update in Update();
            */
            virtual int GetMouseMovementY() = 0;
            /**
            Returns the mousewheelspins and resets the mwheelspins
            TODOXX if there is a problem with the mousewheelspins could be that we are getting it at several places form the module.
            */
            virtual int GetMouseWheelSpins() = 0;
            /**
            Sets the cursor to invisible and in the middle.
            */
            virtual void SetCursorInvisibleAndMiddle(bool p_bool) = 0;
            // TODOEA Test med pekare. FInns med pekare om vi vill speeda upp allt.
            // Tar bort 4 gets per update. o gets från managers.
            // int* GetMouseWheelSpins() { return &m_mouseWheelSpins; }
            //////////////////////////
            /**
            Returns the mousepos x
            */
            virtual int GetMousePosX() = 0;
            /**
            Returns the mousepos y
            */
            virtual int GetMousePosY() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Input::InputModule* (*CREATE_INPUT_MODULE)(const DoremiEngine::Core::SharedContext&);
INPUT_DLL_EXPORT DoremiEngine::Input::InputModule* CreateInputModule(const DoremiEngine::Core::SharedContext& p_context);
}