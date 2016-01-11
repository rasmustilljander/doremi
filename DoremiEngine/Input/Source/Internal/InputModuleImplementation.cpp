#include <Internal/InputModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>
#include <Windows.h>

namespace DoremiEngine
{
    namespace Input
    {
        InputModuleImplementation::InputModuleImplementation() {}

        InputModuleImplementation::~InputModuleImplementation() {}

        void InputModuleImplementation::Startup()
        {
            if(!SDL_WasInit(SDL_INIT_VIDEO))
            {
                if(SDL_Init(SDL_INIT_VIDEO) != 0)
                {
                    std::cout << "SDL_Ubut Error: " << SDL_GetError() << std::endl;
                    SDL_QUIT;
                }
                else
                {
                    // TODO logger maybe
                }
            } // the window to be used
            if(GetActiveWindow() == nullptr)
            {
                CreateWindowSDL(800, 800);
            }
        }

        int InputModuleImplementation::GetMousePosX() { return m_mousePosX; }

        int InputModuleImplementation::GetMousePosY() { return m_mousePosY; }

        void InputModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory) {}

        void InputModuleImplementation::Shutdown() {}

        void InputModuleImplementation::Update()
        {
            // TODOEA COmments that maybe sohuld be removed in the end, it doesnt even matter
            // ResetButtonsDown();
            // ResetMouseMovementStruct();
            // m_mouseWheelSpins = 0;

            SDL_Event eventVariable;
            SwitchCaseEventsForPlaying(eventVariable);
            SDL_GetMouseState(&m_mousePosX, &m_mousePosY);

            // TODEA
            // PrintInputStructsDEBUG();
            // PrintInputMouseMovement();
        }
        int InputModuleImplementation::CreateWindowSDL(int p_width, int p_height)
        {
            // 1200,200 är plats för window
            SDL_Window* t_win = SDL_CreateWindow("Doremi the movie", 1200, 200, p_width, p_height, SDL_WINDOW_SHOWN);
            if(!t_win)
            {
                std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
                SDL_QUIT;
            }
            return 1;
        }
        void InputModuleImplementation::PrintInputStructsDEBUG()
        {
            const size_t t_forLoopSizeI = m_keyboardButtonsDown.size();
            for(size_t i = 0; i < t_forLoopSizeI; ++i)
            {
                std::cout << m_keyboardButtonsDown[i] << ", ";
            }
            const size_t t_forLoopSizeII = m_mouseButtonsDown.size();

            for(size_t i = 0; i < t_forLoopSizeII; ++i)
            {
                std::cout << m_mouseButtonsDown[i] << ", ";
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
        void InputModuleImplementation::PrintInputMouseMovement()
        {
            if(m_mouseMovementStruct.x != 0 || m_mouseMovementStruct.y != 0)
            {
                std::cout << m_mouseMovementStruct.x << " : X, " << m_mouseMovementStruct.y << " : Y" << std::endl;
            }
        }
        int InputModuleImplementation::GetMouseMovementX()
        {
            int t_return = m_mouseMovementStruct.x;
            m_mouseMovementStruct.x = 0;
            return t_return;
        }
        int InputModuleImplementation::GetMouseMovementY()
        {
            int t_return = m_mouseMovementStruct.y;
            m_mouseMovementStruct.y = 0;
            return t_return;
        }
        int InputModuleImplementation::GetMouseWheelSpins()
        {
            int t_return = m_mouseWheelSpins;
            m_mouseWheelSpins = 0;
            return t_return;
        }
        void InputModuleImplementation::SetCursorInvisibleAndMiddle(bool p_bool)
        {
            if (p_bool)
            {
                SDL_SetRelativeMouseMode(SDL_TRUE);//SDL_WarpMouseGlobal(int x, int y); skulle kunna använda detta för då kan nog musen synas!
            }
            else
            {
                SDL_SetRelativeMouseMode(SDL_FALSE);//Disabla musen fungerar SDL_ShowCursor(SDL_DISABLE);, verkar inte riktigt fungera vid SDL_TRUE
            }
        }
        void InputModuleImplementation::SwitchCaseEventsForPlaying(SDL_Event& p_eventVariable)
        {
            while(SDL_PollEvent(&p_eventVariable))
            {
                switch(p_eventVariable.type)
                {

                    case SDL_KEYDOWN:
                        AddToList(p_eventVariable.key.keysym.sym, m_keyboardButtonsDown);
                        break;

                    case SDL_KEYUP:
                        RemoveFromList(p_eventVariable.key.keysym.sym, m_keyboardButtonsDown);
                        break;

                    case SDL_MOUSEBUTTONDOWN:
                        AddToList(p_eventVariable.button.button, m_mouseButtonsDown);
                        break;

                    case SDL_MOUSEBUTTONUP:
                        RemoveFromList(p_eventVariable.button.button, m_mouseButtonsDown);
                        break;

                    case SDL_MOUSEMOTION:
                        m_mouseMovementStruct.x = m_mouseMovementStruct.x + p_eventVariable.motion.xrel;
                        m_mouseMovementStruct.y = m_mouseMovementStruct.y + p_eventVariable.motion.yrel;
                        break;

                    case SDL_MOUSEWHEEL:
                        m_mouseWheelSpins = m_mouseWheelSpins + p_eventVariable.wheel.y;
                        break;

                    default:
                        break;
                }
            }
        }
        void InputModuleImplementation::SwitchCaseEventsForTyping(SDL_Event& p_eventVariable)
        {
            while(SDL_PollEvent(&p_eventVariable))
            {
                switch(p_eventVariable.type)
                {

                    case SDL_KEYDOWN: // kan va bra om vi ska skriva text!
                        // AddToList(eventVariable.key.keysym.sym, );
                        break;

                    case SDL_KEYUP:
                        // RemoveFromList(eventVariable.key.keysym.sym);
                        break;

                    case SDL_MOUSEBUTTONDOWN:
                        m_mouseButtonsDown.push_back(p_eventVariable.button.button);
                        break;


                    case SDL_MOUSEMOTION:
                        m_mouseMovementStruct.x = p_eventVariable.motion.xrel;
                        m_mouseMovementStruct.y = p_eventVariable.motion.yrel;
                        break;

                    case SDL_MOUSEWHEEL: // TODEA Fix mousewheelshit för båda eventcaseshitsen
                        m_mouseWheelSpins = p_eventVariable.wheel.y;
                        break;

                    default:
                        break;
                }
            }
        }
        void InputModuleImplementation::ResetMouseMovementStruct()
        {
            m_mouseMovementStruct.x = 0;
            m_mouseMovementStruct.y = 0;
        }
        void InputModuleImplementation::ResetButtonsDown()
        {
            m_keyboardButtonsDown.clear();
            m_mouseButtonsDown.clear();
        }
        void InputModuleImplementation::RemoveFromList(SDL_Keycode p_eventvariable, std::vector<int>& o_listToUse)
        {
            size_t t_sizeOfVector = o_listToUse.size();
            for(size_t i = 0; i < t_sizeOfVector; ++i)
            {
                if(o_listToUse[i] == p_eventvariable)
                {
                    o_listToUse.erase(o_listToUse.begin() + i);
                    --t_sizeOfVector;
                }
            }
        }
        void InputModuleImplementation::AddToList(SDL_Keycode p_eventvariable, std::vector<int>& o_listToUse)
        {
            const size_t t_sizeOfVector = o_listToUse.size();
            bool t_doesItAlreadyExist = false;
            for(size_t i = 0; i < t_sizeOfVector; ++i)
            {
                if(o_listToUse[i] == p_eventvariable)
                {
                    t_doesItAlreadyExist = true;
                }
            }
            if(!t_doesItAlreadyExist)
            {
                o_listToUse.push_back(p_eventvariable);
            }
        }
    }
}

DoremiEngine::Input::InputModule* CreateInputModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Input::InputModule* input = new DoremiEngine::Input::InputModuleImplementation();
    return input;
}