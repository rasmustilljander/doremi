#include <InputHandlerClient.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>

// Timing
#include <DoremiEngine\Timing\Include\Measure\MeasureTimer.hpp>

#include <iostream>

namespace Doremi
{
    namespace Core
    {
        InputHandlerClient::InputHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : InputHandler(p_sharedContext)
        {
            // Setup the different inputmoves! (Used in the game)
            // When inserting something here please fill out in the enum what key is is and what code it is.(In input handler)
            // TODOCONFIG ladda in vad alla värden och vilken action de tillhör.
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
            userCmdsPlaying[1] = UserCommandPlaying::LeftClick;
            userCmdsPlaying[55] = UserCommandPlaying::SetFrequency0;
            userCmdsPlaying[56] = UserCommandPlaying::SetFrequency500;
            userCmdsPlaying[57] = UserCommandPlaying::SetFrequency1000;
            userCmdsPlaying[3] = UserCommandPlaying::RightClick;

            // Setup for the menuinput
            // TODOEA Not used yet.
            userCmdsMeny[1] = UserCommandMeny::LeftClick;
            userCmdsMeny[3] = UserCommandMeny::RightClick;
            userCmdsMeny[13] = UserCommandMeny::Enter;
            userCmdsMeny[1073741906] = UserCommandMeny::Up;
            userCmdsMeny[1073741905] = UserCommandMeny::Down;
            userCmdsMeny[1073741904] = UserCommandMeny::Left;
            userCmdsMeny[1073741903] = UserCommandMeny::Right;
        }

        void InputHandlerClient::SetCursorInvisibleAndMiddle(bool p_bool) { m_sharedContext.GetInputModule().SetCursorInvisibleAndMiddle(p_bool); }

        void InputHandlerClient::GetMousePos(int& p_positionX, int& p_positionY)
        {
            // Recieves the mousepos from input module.
            p_positionX = m_sharedContext.GetInputModule().GetMousePosX();
            p_positionY = m_sharedContext.GetInputModule().GetMousePosY();
        }

        void InputHandlerClient::BuildMaskFromEngineForGame()
        {
            size_t t_sizeVector = m_keyboardInputFromModule.size();
            m_maskWithInput = 0;
            // Checks if the inputs from input module contain any relevant keystrokes using the keyboard.
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            // checks if the inputs from the input module contain any relevant keystrokes using the mouse.
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_musInputFromModule[i]];
            }
            // Just checks if the mousewheel has been scrolled. I think 999 and 998 is harc coded values that i reserved for mousewheel.
            if(m_mouseWheelInput) 
            {
                // if it is above zero it is considered as mwheel up
                if(m_mouseWheelInput > 0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[999];
                }
                // if it is below zero it is considered as mwheel down
                else if(m_mouseWheelInput < 0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[998];
                }
            }
        }

        void InputHandlerClient::BuildMaskFromEngineForMeny()
        {
            size_t t_sizeVector = m_keyboardInputFromModule.size();
            // Checks if the inputs from input module contain any relevant keystrokes using the keyboard.
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsMeny[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            // checks if the inputs from the input module contain any relevant keystrokes using the mouse.
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsMeny[m_musInputFromModule[i]];
            }
        }

        void InputHandlerClient::UpdateInputsFromEngine()
        {
            TIME_FUNCTION_START
            // Recieves the following things from the Input Module.
            m_mouseMoveX = m_sharedContext.GetInputModule().GetMouseMovementX();
            m_mouseMoveY = m_sharedContext.GetInputModule().GetMouseMovementY();
            m_musInputFromModule = m_sharedContext.GetInputModule().GetMouseButtonInput();
            m_keyboardInputFromModule = m_sharedContext.GetInputModule().GetKeyBoardInput();
            m_mouseWheelInput = m_sharedContext.GetInputModule().GetMouseWheelSpins();
            TIME_FUNCTION_STOP
        }

        void InputHandlerClient::PrintInputStructsDEBUG()
        {
            // Debug purpose pick the thing you want/don't want to be printed and decomment/comment
            const size_t t_forLoopSizeI = m_keyboardInputFromModule.size();
            // Keyboard
            for(size_t i = 0; i < t_forLoopSizeI; ++i)
            {
                std::cout << m_keyboardInputFromModule[i] << ", ";
            }
            const size_t t_forLoopSizeII = m_musInputFromModule.size();
            // MouseButtons
            for(size_t i = 0; i < t_forLoopSizeII; ++i)
            {
                std::cout << m_musInputFromModule[i] << ", ";
            }
            if(t_forLoopSizeII || t_forLoopSizeI)
            {
                std::cout << std::endl;
            }
            // if (t_printDebug)
            //{
            //    for (size_t i = 0; i < m_structArraySize; ++i)
            //    {
            //        std::cout << m_inputForPlaying.keys[i];
            //    }
            //    std::cout << std::endl;
            //}
            // Mousewheel
            // if (m_mouseWheelSpins != 0)
            //{
            //    std::cout << m_mouseWheelSpins << std::endl;
            //}
            // Amount of keyboardbuttons pressed
            // std::cout << m_keyboardButtonsDown.size() << std::endl;
            // Amount of mousebuttons pressed
            // std::cout << m_mouseButtonsDown.size() << " : MouseInputs. " << m_keyboardButtonsDown.size() << " : KeyboardInputs" << std::endl;
        }

        void InputHandlerClient::PrintInputMouseMovement()
        {
            // Debug for showing the mousemovement
            if(m_mouseMoveX != 0 || m_mouseMoveY != 0)
            {
                std::cout << m_mouseMoveX << " : X, " << m_mouseMoveY << " : Y" << std::endl;
            }
        }

        void InputHandlerClient::Update()
        {
            TIME_FUNCTION_START
            UpdateInputsFromEngine();
            m_lastUpdateMaskWithInput = m_maskWithInput;
            BuildMaskFromEngineForGame();
            // PrintInputStructsDEBUG();
            // PrintInputMouseMovement();
            TIME_FUNCTION_STOP
        }

        void InputHandlerClient::ChangeThisKeyToThat(int p_bitMask)
        {
            // TODEA fixa keyconfiggen va
        }
    }
}