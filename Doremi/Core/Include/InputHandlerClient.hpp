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
                TODOEA doc
            */
            void SetCursorInvisibleAndMiddle(bool p_bool);

            /**
                TODOEA doc
            */
            void Update();

            /**
                TODOEA doc
            */
            void ChangeThisKeyToThat(int p_bitMask);

            /**
                Return from InputModule with a changed value from mousesense
            */
            const int GetMouseMovementX() const { return m_mouseMoveX * m_mouseSense; }

            /**
                Return from InputModule with a changed value from mousesense
            */
            const int GetMouseMovementY() const { return m_mouseMoveY * m_mouseSense; }

        private:
            /**
                TODOEA doc
            */
            void UpdateInputsFromEngine();

            /**
                TODOEA doc
            */
            void BuildMaskFromEngineForGame();

            /**
                TODOEA doc
            */
            void BuildMaskFromEngineForMeny();

            /**
                TODOEA doc
            */
            void PrintInputMouseMovement();

            /**
                TODOEA doc
            */
            void PrintInputStructsDEBUG();

            /**
                TODOEA doc
            */
            int m_mouseSense = 1;

            /**
                TODOEA doc
            */
            int m_mouseMoveX = 0;

            /**
                TODOEA doc
            */
            int m_mouseMoveY = 0;

            /**
                TODOEA doc
            */
            int m_mouseWheelInput = 0;

            // TODOEA går det att sätta pekare istället så man inte behöver get'a
            /**
                TODOEA doc
            */
            std::vector<int> m_musInputFromModule;

            /**
                TODOEA doc
            */
            std::vector<int> m_keyboardInputFromModule;

            /**
                TODOEA doc
            */
            std::unordered_map<int, UserCommandPlaying> userCmdsPlaying;

            /**
                TODOEA doc
            */
            std::unordered_map<int, UserCommandMeny> userCmdsMeny;
        };
    }
}