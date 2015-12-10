#pragma once
// Project specific
#include <InputHandler.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>

namespace Doremi
{
    namespace Core
    {
        InputHandler::InputHandler()
        {
        }


        InputHandler::~InputHandler()
        {
        }
        void InputHandler::Initialize()
        {
            //Setup the different inputmoves! (Used in the game)
            userCmdsPlaying[32] = UserCommandPlaying::Jump;
            userCmdsPlaying[119] = UserCommandPlaying::Forward;
            userCmdsPlaying[116] = UserCommandPlaying::Backward;
            userCmdsPlaying[97] = UserCommandPlaying::Left;
            userCmdsPlaying[100] = UserCommandPlaying::Right;
            userCmdsPlaying[1] = UserCommandPlaying::Fire;
            userCmdsPlaying[999] = UserCommandPlaying::ScrollWpnUp;
            userCmdsPlaying[998] = UserCommandPlaying::ScrollWpnDown;
            
            //Setup for the menyinput
            userCmdsMeny[1] = UserCommandMeny::LeftClick;
            userCmdsMeny[3] = UserCommandMeny::RightClick;
            userCmdsMeny[13] = UserCommandMeny::Enter;
            userCmdsMeny[1073741906] = UserCommandMeny::Up;
            userCmdsMeny[1073741905] = UserCommandMeny::Down;
            userCmdsMeny[1073741904] = UserCommandMeny::Left;
            userCmdsMeny[1073741903] = UserCommandMeny::Right;

            /////// TODOEA TESTSKIT!
           //m_keyboardInputFromModule.push_back(8);
           //m_keyboardInputFromModule.push_back(97);
           //m_keyboardInputFromModule.push_back(119);
           //m_keyboardInputFromModule.push_back(100);
           //m_keyboardInputFromModule.push_back(97);
           //m_keyboardInputFromModule.push_back(116);
           //m_keyboardInputFromModule.push_back(116);
           //m_musInputFromModule.push_back(1);
           //m_musInputFromModule.push_back(2);
           //m_musInputFromModule.push_back(3);
           //m_mouseWheelInput = -1;
            ChangeThisKeyToThat(2);


            //BuildMaskFromEngine();
            //CheckBitMaskInputFromGame((int)UserCommand::Backward|(int)UserCommand::Fire);
        }
        bool InputHandler::CheckBitMaskInputFromGame(int p_bitMask)//Send what to check from
        {
            if ((m_maskWithInput & p_bitMask) == p_bitMask)
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
            if (m_singleton == nullptr)
            {
                m_singleton = new InputHandler();
            }
            return m_singleton;


        }
        void InputHandler::BuildMaskFromEngineForGame()
        {
            int t_sizeVector = m_keyboardInputFromModule.size();

            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_keyboardInputFromModule[i]];
            }

            t_sizeVector = m_musInputFromModule.size();
            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[m_musInputFromModule[i]];
            }
            if (m_mouseWheelInput)
            {
                if (m_mouseWheelInput>0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[999];

                }
                else if (m_mouseWheelInput<0)
                {
                    m_maskWithInput = m_maskWithInput | (int)userCmdsPlaying[998];
                }
            }
        }

        void InputHandler::BuildMaskFromEngineForMeny()
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

        void InputHandler::UpdateInputsFromEngine(const DoremiEngine::Core::SharedContext & p_sharedContext)
        {
            m_mouseMoveX = p_sharedContext.GetInputModule().GetMouseMovementX();
            m_mouseMoveY = p_sharedContext.GetInputModule().GetMouseMovementY();
            m_musInputFromModule = p_sharedContext.GetInputModule().GetMouseButtonInput();
            m_keyboardInputFromModule = p_sharedContext.GetInputModule().GetKeyBoardInput();
            m_mouseWheelInput = p_sharedContext.GetInputModule().GetMouseWheelSpins();
        }

        void InputHandler::Update(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            UpdateInputsFromEngine(p_sharedContext);
            //Do one of them TODOEA
            BuildMaskFromEngineForGame();
            BuildMaskFromEngineForMeny();
        }
        void InputHandler::ChangeThisKeyToThat(int p_bitMask)
        {

        }
    }
}
