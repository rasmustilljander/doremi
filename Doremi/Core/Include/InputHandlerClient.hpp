#pragma once
#include <Doremi/Core/Include/InputHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        class InputHandlerClient : public InputHandler
        {
        public:
            /**
                TODOCM doc
            */
            InputHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                Talks to Inputmodule and sets the settings as the method name.
            */
            void SetCursorInvisibleAndMiddle(bool p_bool);

            /**
                Updates the m_lastUpdateMaskWithInput, does not reset the m_maskWIthInput (I think the InputModule fixes that).
            */
            void Update();

            /**
                Will fix the keyconfig.
            */
            void ChangeThisKeyToThat(int p_bitMask);

            /**
                Return from InputModule with a changed value from mousesense. The change in keypos since last update.
            */
            const int GetMouseMovementX() const { return m_mouseMoveX * m_mouseSense; }

            /**
                Return from InputModule with a changed value from mousesense. The change in keypos since last update.
            */
            const int GetMouseMovementY() const { return m_mouseMoveY * m_mouseSense; }

            /**
                Recieves the mousepos from input module.
            */
            void GetMousePos(int& p_positionX, int& p_positionY);


        private:
            /**
                Updates values from InputModue
            */
            void UpdateInputsFromEngine();

            /**
                Uses the mask recieved from the InputModule and checks if the values are of interest. The values of interest are the values in
               userCmdsPlaying.
            */
            void BuildMaskFromEngineForGame();

            /**
                Does the same for menu. Not used for now.
            */
            void BuildMaskFromEngineForMeny();

            /**
                Debug, prints out the mousex and mousey recieved from the input module.
            */
            void PrintInputMouseMovement();

            /**
                Debug, prints out the input struct recieved form input module.
            */
            void PrintInputStructsDEBUG();

            /**
                What mousesense is used. TODOCONFIG
            */
            int m_mouseSense = 1;

            /**
                Value recieved from input module. How much the mouse moved since last update in x.
            */
            int m_mouseMoveX = 0;

            /**
                Value recieved from input module. How much the mouse moved since last update in y.
            */
            int m_mouseMoveY = 0;

            /**
                How many times the mousewheel turned since last update. Recieved from input module.
            */
            int m_mouseWheelInput = 0;

            // TODOEA går det att sätta pekare istället så man inte behöver get'a
            /**
                A vector for mouseinput. The different buttonclicks on the mouse.
            */
            std::vector<int> m_musInputFromModule;

            /**
                Values recieved from input module. The int value for the key pressed.
            */
            std::vector<int> m_keyboardInputFromModule;

            /**
                The interesting keys that we will look for when using the input. The keys that are not in this list will not be note worthy.
            */
            std::unordered_map<int, UserCommandPlaying> userCmdsPlaying;

            /**
                Same for menu, not implemented yet TODOEA
            */
            std::unordered_map<int, UserCommandMeny> userCmdsMeny;
        };
    }
}