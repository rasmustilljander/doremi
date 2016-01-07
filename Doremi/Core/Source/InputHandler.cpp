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
            userCmdsPlaying[228] = UserCommandPlaying::StartRepeatableAudioRecording;
            userCmdsPlaying[246] = UserCommandPlaying::PlayRepeatableAudioRecording;
        }

        InputHandler::~InputHandler() 
        {

        }

        bool InputHandler::CheckBitMaskInputFromGame(int p_bitMask)
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

        bool InputHandler::CheckForOnePress(int p_bitMask)
        {
            if(CheckBitMaskInputFromGame(p_bitMask))
            {
                if((m_lastUpdateMaskWithInput & p_bitMask) == p_bitMask)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                return false;
            }

            return false;
        }

        uint32_t InputHandler::GetInputBitMask()
        {
            return m_maskWithInput;
        }

        void InputHandler::SetInputBitMask(uint32_t p_bitMask)
        {
            m_maskWithInput = p_bitMask;
        }
    }
}
