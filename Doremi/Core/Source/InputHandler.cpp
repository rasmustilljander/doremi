#pragma once
// Project specific
#include <InputHandler.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <iostream>

// Timing
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

namespace Doremi
{
    namespace Core
    {
        InputHandler::InputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        InputHandler::~InputHandler() {}

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

        uint32_t InputHandler::GetInputBitMask() { return m_maskWithInput; }
    }
}
