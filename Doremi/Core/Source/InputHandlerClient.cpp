#include <InputHandlerClient.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>

#include <iostream>

namespace Doremi
{
    namespace Core
    {
        InputHandlerClient::InputHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : InputHandler(p_sharedContext)
        {
            // Setup the different inputmoves! (Used in the game)
            userCmdsPlaying[32] = UserCommandPlaying::Jump;
            userCmdsPlaying[119] = UserCommandPlaying::Forward;
            userCmdsPlaying[115] = UserCommandPlaying::Backward;
            userCmdsPlaying[97] = UserCommandPlaying::Left;
            userCmdsPlaying[100] = UserCommandPlaying::Right;
            userCmdsPlaying[1] = UserCommandPlaying::Fire;
            userCmdsPlaying[999] = UserCommandPlaying::ScrollWpnUp;
            userCmdsPlaying[998] = UserCommandPlaying::ScrollWpnDown;
            userCmdsPlaying[1073741906] = UserCommandPlaying::DebugForward;
            userCmdsPlaying[1073741905] = UserCommandPlaying::DebugBackward;
            userCmdsPlaying[1073741904] = UserCommandPlaying::DebugLeft;
            userCmdsPlaying[1073741903] = UserCommandPlaying::DebugRight;
            userCmdsPlaying[120] = UserCommandPlaying::DebugButton;
            userCmdsPlaying[228] = UserCommandPlaying::StartRepeatableAudioRecording;
            userCmdsPlaying[246] = UserCommandPlaying::PlayRepeatableAudioRecording;
            userCmdsPlaying[27] = UserCommandPlaying::ExitGame;

            // Setup for the menyinput
            userCmdsMeny[1] = UserCommandMeny::LeftClick;
            userCmdsMeny[3] = UserCommandMeny::RightClick;
            userCmdsMeny[13] = UserCommandMeny::Enter;
            userCmdsMeny[1073741906] = UserCommandMeny::Up;
            userCmdsMeny[1073741905] = UserCommandMeny::Down;
            userCmdsMeny[1073741904] = UserCommandMeny::Left;
            userCmdsMeny[1073741903] = UserCommandMeny::Right;
        }

        void InputHandlerClient::SetCursorInvisibleAndMiddle(bool p_bool)
        {
            m_sharedContext.GetInputModule().SetCursorInvisibleAndMiddle(p_bool);
        }

        void InputHandlerClient::BuildMaskFromEngineForGame()
        {
            int t_sizeVector = m_keyboardInputFromModule.size();
            m_maskWithInput = 0;
            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_musInputFromModule[i]];
            }
            if (m_mouseWheelInput) // TODOEA move to different function that wont update each frame
            {
                if (m_mouseWheelInput > 0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[999];
                }
                else if (m_mouseWheelInput < 0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[998];
                }
            }
        }

        void InputHandlerClient::BuildMaskFromEngineForMeny()
        {
            int t_sizeVector = m_keyboardInputFromModule.size();

            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsMeny[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsMeny[m_musInputFromModule[i]];
            }
        }

        void InputHandlerClient::UpdateInputsFromEngine()
        {
            m_mouseMoveX = m_sharedContext.GetInputModule().GetMouseMovementX();
            m_mouseMoveY = m_sharedContext.GetInputModule().GetMouseMovementY();
            m_musInputFromModule = m_sharedContext.GetInputModule().GetMouseButtonInput();
            m_keyboardInputFromModule = m_sharedContext.GetInputModule().GetKeyBoardInput();
            m_mouseWheelInput = m_sharedContext.GetInputModule().GetMouseWheelSpins();
        }

        void InputHandlerClient::PrintInputStructsDEBUG()
        {
            int t_forLoopSizeI = m_keyboardInputFromModule.size();
            for (size_t i = 0; i < t_forLoopSizeI; ++i)
            {
                std::cout << m_keyboardInputFromModule[i] << ", ";
            }
            int t_forLoopSizeII = m_musInputFromModule.size();

            for (size_t i = 0; i < t_forLoopSizeII; ++i)
            {
                std::cout << m_musInputFromModule[i] << ", ";
            }
            if (t_forLoopSizeII || t_forLoopSizeI)
            {
                std::cout << std::endl;
            }
            // bool t_printDebug = false;
            // for (size_t i = 0; i < m_structArraySize; ++i)
            //{
            //    if (m_inputForPlaying.keys[i])
            //    {
            //        t_printDebug = true;
            //    }

            //}
            // if (t_printDebug)
            //{
            //    for (size_t i = 0; i < m_structArraySize; ++i)
            //    {
            //        std::cout << m_inputForPlaying.keys[i];
            //    }
            //    std::cout << std::endl;
            //}
            // if (m_mouseWheelSpins != 0)
            //{
            //    std::cout << m_mouseWheelSpins << std::endl;
            //}

            // std::cout << m_keyboardButtonsDown.size() << std::endl;
            // std::cout << m_mouseButtonsDown.size() << " : MouseInputs. " << m_keyboardButtonsDown.size() << " : KeyboardInputs" << std::endl;
        }

        void InputHandlerClient::PrintInputMouseMovement()
        {
            if (m_mouseMoveX != 0 || m_mouseMoveY != 0)
            {
                std::cout << m_mouseMoveX << " : X, " << m_mouseMoveY << " : Y" << std::endl;
            }
        }

        void InputHandlerClient::Update()
        {
            UpdateInputsFromEngine();
            m_lastUpdateMaskWithInput = m_maskWithInput;
            // Do one of them TODOEA
            BuildMaskFromEngineForGame();
            // BuildMaskFromEngineForMeny();

            // PrintInputStructsDEBUG();
            // PrintInputMouseMovement();
        }

        void InputHandlerClient::ChangeThisKeyToThat(int p_bitMask)
        {
            // TODEA fixa keyconfiggen va
        }
    }
}