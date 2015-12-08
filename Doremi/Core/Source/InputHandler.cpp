#pragma once
// Project specific
#include "InputHandler.hpp"

namespace Doremi
{
	namespace Core
	{
        InputHandler* InputHandler::m_singleton = nullptr;

        InputHandler* InputHandler::GetInstance()
		{
			if (m_singleton == nullptr)
			{
				m_singleton = new InputHandler();
			}
			return m_singleton;
		}

        InputHandler::InputHandler()
		{
		}


        InputHandler::~InputHandler()
		{
		}
        void InputHandler::CheckInputsFromEngine()
        {

            t_keyboardInputFromModule.push_back(10);
            t_keyboardInputFromModule.push_back(1);

            int t_sizeVector = t_keyboardInputFromModule.size();

            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmds[t_keyboardInputFromModule[i]];
            }

            t_sizeVector = t_musInputFromModule.size();
            for (size_t i = 0; i < t_sizeVector; ++i)
            {
                m_maskWithInput = m_maskWithInput | (int)userCmds[t_musInputFromModule[i]];
            }
        }
	}
}
