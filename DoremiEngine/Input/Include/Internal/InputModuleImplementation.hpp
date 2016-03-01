#pragma once
#include <unordered_map>
#include <InputModule.hpp>
#include <vector>
#include <SDL2/SDL.h>


namespace DoremiEngine
{
    namespace Input
    {
        class InputModuleImplementation : public InputModule
        {
        private:
            /**
             The struct used for storing the mousemovement since last update.
            */
            struct MouseMovementStruct
            {
                int x = 0;
                int y = 0;
            };

            MouseMovementStruct m_mouseMovementStruct;
            std::vector<int> m_keyboardButtonsDown;
            std::vector<int> m_mouseButtonsDown; // will be sent if the target wants the buttons of the mouse(no scrolling)
            int m_mouseWheelSpins = 0;

        public:
            /**
                TODO docs
            */
            InputModuleImplementation();

            /**
                TODO docs
            */
            virtual ~InputModuleImplementation();

            /**
                Returns the X position of mouse
            */
            int GetMousePosX() override;

            /**
                Returns the Y position of mouse
            */
            int GetMousePosY() override;

            /**
                Initializes SDL and creates window if there isn't a window active.
            */
            void Startup() override;

            /**
            TODO Does nothing
            */
            void Shutdown() override;
            /**
            Resets the mousemovement struct, gets the keystrokes pressed and gets the mouseposition.
            */
            void Update() override;
            /**
            Create a window using sdl. Returns 1 if a window is created
            */
            int CreateWindowSDL(int p_width, int p_height);
            /**
            Debug, Prints out the struct recieved form sdl.
            */
            void PrintInputStructsDEBUG();
            /**
            Debug, Prints out the struct recieved form sdl.
            */
            void PrintInputMouseMovement();
            /**
            Get function, so the input handler can get it.
            */
            const std::vector<int> GetKeyBoardInput() const { return m_keyboardButtonsDown; }
            /**
            Get function, so the input handler can get it.
            */
            const std::vector<int> GetMouseButtonInput() const { return m_mouseButtonsDown; }
            /**
            The mousemovement in x since last update, this value is reset at the start of the update in Update();
            */
            int GetMouseMovementX();
            /**
            The mousemovement in y since last update, this value is reset at the start of the update in Update();
            */
            int GetMouseMovementY();
            /**
            Returns the mousewheelspins and resets the mwheelspins
            TODOXX if there is a problem with the mousewheelspins could be that we are getting it at several places form the module.
            */
            int GetMouseWheelSpins();
            /**
            Sets the cursor to invisible and in the middle.
            */
            void SetCursorInvisibleAndMiddle(bool p_bool);

            void SetExitFunction(std::function<void()> p_function) override;

        private:
            // SDL_Window *m_win;
            const Uint8* m_keyState;
            /**
                We check for events using sdl if a mousebutton is pressed/released if the mousewheel is spun or if any keyboardkeys are
               pressed/released
            */
            void SwitchCaseEventsForPlaying(SDL_Event& p_eventVariable);
            /**
            Not used so far.
            */
            void SwitchCaseEventsForTyping(SDL_Event& p_eventVariable);
            /**
                Reset the mousemovement every update.
            */
            void ResetMouseMovementStruct();

            /**
            Checks if the Keycode already exists in the list. If it doesnt it adds it.
            */
            void AddToList(SDL_Keycode p_eventvariable, std::vector<int>& o_listToUse);
            /**
            Checks if the keycode exists in the list and if it does. We remove it form the list.
            */
            void RemoveFromList(SDL_Keycode p_eventvariable, std::vector<int>& o_listToUse);

            int m_mousePosX;
            int m_mousePosY;

            std::function<void()> m_exitFunction;
        };
    }
}