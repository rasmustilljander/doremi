#pragma once
// Project specific
#include <InputHandler.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        InputHandler::InputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) 
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

            // Setup for the menyinput
            userCmdsMeny[1] = UserCommandMeny::LeftClick;
            userCmdsMeny[3] = UserCommandMeny::RightClick;
            userCmdsMeny[13] = UserCommandMeny::Enter;
            userCmdsMeny[1073741906] = UserCommandMeny::Up;
            userCmdsMeny[1073741905] = UserCommandMeny::Down;
            userCmdsMeny[1073741904] = UserCommandMeny::Left;
            userCmdsMeny[1073741903] = UserCommandMeny::Right;
        }


        InputHandler::~InputHandler() {}
        void InputHandler::StartInputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            m_singleton = new InputHandler(p_sharedContext);
        }
        void InputHandler::Initialize()
        {

        }
        bool InputHandler::CheckBitMaskInputFromGame(int p_bitMask) // Send what to check from
        {
            if((m_maskWithInput & p_bitMask) == p_bitMask)
            {
                return true;
            }
            else
            {
                return false;
            }
            return false;
        }
        InputHandler* InputHandler::m_singleton = nullptr;

        InputHandler* InputHandler::GetInstance()
        {
            // if (m_singleton == nullptr)
            //{
            //    m_singleton = new InputHandler();
            //}
            return m_singleton;
        }
        void InputHandler::BuildMaskFromEngineForGame()
        {
            int t_sizeVector = m_keyboardInputFromModule.size();
            m_maskWithInput = 0;
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_musInputFromModule[i]];
            }
            if(m_mouseWheelInput)
            {
                if(m_mouseWheelInput > 0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[999];
                }
                else if(m_mouseWheelInput < 0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[998];
                }
            }
        }

        void InputHandler::BuildMaskFromEngineForMeny()
        {
            int t_sizeVector = m_keyboardInputFromModule.size();

            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsMeny[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            for(size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsMeny[m_musInputFromModule[i]];
            }
        }

        void InputHandler::UpdateInputsFromEngine()
        {
            m_mouseMoveX = m_sharedContext.GetInputModule().GetMouseMovementX();
            m_mouseMoveY = m_sharedContext.GetInputModule().GetMouseMovementY();
            m_musInputFromModule = m_sharedContext.GetInputModule().GetMouseButtonInput();
            m_keyboardInputFromModule = m_sharedContext.GetInputModule().GetKeyBoardInput();
            m_mouseWheelInput = m_sharedContext.GetInputModule().GetMouseWheelSpins();
        }
        void InputHandler::PrintInputStructsDEBUG()
        {
            int t_forLoopSizeI = m_keyboardInputFromModule.size();
            for(size_t i = 0; i < t_forLoopSizeI; ++i)
            {
                std::cout << m_keyboardInputFromModule[i] << ", ";
            }
            int t_forLoopSizeII = m_musInputFromModule.size();

            for(size_t i = 0; i < t_forLoopSizeII; ++i)
            {
                std::cout << m_musInputFromModule[i] << ", ";
            }
            if(t_forLoopSizeII || t_forLoopSizeI)
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
        void InputHandler::PrintInputMouseMovement()
        {
            if(m_mouseMoveX != 0 || m_mouseMoveY != 0)
            {
                std::cout << m_mouseMoveX << " : X, " << m_mouseMoveY << " : Y" << std::endl;
            }
        }
        void InputHandler::Update()
        {
            UpdateInputsFromEngine();

            // Do one of them TODOEA
            BuildMaskFromEngineForGame();
            //BuildMaskFromEngineForMeny();

            // PrintInputStructsDEBUG();
            // PrintInputMouseMovement();
        }
        void InputHandler::ChangeThisKeyToThat(int p_bitMask)
        {
            // TODEA fixa keyconfiggen va
        }
    }
}
