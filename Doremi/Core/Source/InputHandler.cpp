#pragma once
// Project specific
#include <InputHandler.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        InputHandler::InputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        InputHandler::~InputHandler() {}

        bool InputHandler::CheckBitMaskInputFromGame(int p_bitMask)
        {
            // Checks if the desired key is pushed.
            if((m_maskWithInput & p_bitMask) == p_bitMask)
            {
                return true;
            }
            else
            {
                return false;
            }
            // If it doesn't find what it is looking for it will return false.
            return false;
        }

        bool InputHandler::CheckForOnePress(int p_bitMask)
        {
            // Using the other function to check if it is the desired key.
            if(CheckBitMaskInputFromGame(p_bitMask))
            {
                // Checks if the key was down last update
                if((m_lastUpdateMaskWithInput & p_bitMask) == p_bitMask)
                {
                    // If the key was pressed last update then this is not the first time it is pressed this keystroke and will return a false.
                    return false;
                }
                else
                {
                    // If the key wasn't pressed last update it is the first time update being down and will return a true.
                    return true;
                }
            }
            else
            {
                return false;
            }

            return false;
        }

        bool InputHandler::CheckForRelease(int p_bitMask)
        {
            // Check if we pressed previous frame
            if((m_lastUpdateMaskWithInput & p_bitMask) == p_bitMask)
            {
                // If we didn't press this frame, it's a release
                if(!CheckBitMaskInputFromGame(p_bitMask))
                {
                    return true;
                }
                else // We still be pressin
                {
                    return false;
                }
            }
            else // We didn't press to begin with
            {
                return false;
            }
        }

        uint32_t InputHandler::GetInputBitMask() { return m_maskWithInput; }
    }
}
