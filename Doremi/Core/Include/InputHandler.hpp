#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        enum class UserCommandPlaying

        // TODOEA Lägga in detta i en textfil.
        { // Key       Code for it
            Jump = 1, // Space     32
            Forward = 2, // W         119
            Backward = 4, // S         115
            Left = 8, // A         97
            Right = 16, // D         100
            Fire = 32, // LeftMouseClick   1
            ScrollWpnUp = 64, // MWheelUp    NULL Handled differently
            ScrollWpnDown = 128, // MWheelDown NULL Handled differently
            DebugForward = 256, // 1073741906
            DebugBackward = 512, // 1073741905
            DebugLeft = 1024, // 1073741904
            DebugRight = 2048, // 1073741903
            DebugButton = 4096,
            StartRepeatableAudioRecording = 8192,
            PlayRepeatableAudioRecording = 16384,
            All = Jump | Forward | Backward | Left | Right | Fire | ScrollWpnUp | ScrollWpnDown, // Seen this be done so doing it here aswell! =D
        };
        enum class UserCommandMeny

        // TODOEA Lägga in detta i en textfil.
        { // Code
            LeftClick = 1, // 1
            RightClick = 2, // 3
            Enter = 4, // 13
            Up = 8, // 1073741906
            Down = 16, // 1073741905
            Left = 32, // 1073741904
            Right = 64, // 1073741903
        };

        class InputHandler
        {
            public:
            /** Is a singleton. Use this method to get the EventManager*/
            static InputHandler* GetInstance();
            InputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~InputHandler();
            static void StartInputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            void Initialize();
            bool CheckBitMaskInputFromGame(int p_bitMask);
            bool CheckForOnePress(int p_bitMask);
            // return from InputModule with a changed value from mousesense
            const int GetMouseMovementX() const { return m_mouseMoveX * m_mouseSense; }
            const int GetMouseMovementY() const { return m_mouseMoveY * m_mouseSense; }
            //
            void SetCursorInvisibleAndMiddle(bool p_bool);
            // void ChangeKeyConfig();//Får se hur vi gör här kan göra på flera sätt.
            // Kan göra att jag har massa funktioner här eller att den menyn skickar in ett ID som
            // på vad som ska bytas så kan vi koppla det på något SKÖNT sätt ;)
            // Behöver nog ta bort old entries eller ändra dem på något sätt.
            void Update();
            void ChangeThisKeyToThat(int p_bitMask);

            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            void UpdateInputsFromEngine();
            std::unordered_map<int, UserCommandPlaying> userCmdsPlaying;
            std::unordered_map<int, UserCommandMeny> userCmdsMeny;
            int m_mouseSense = 1;
            // TODOEA Could be a problem with meny and game inputs I DUNNO To only hav one bitmask
            int m_maskWithInput = 0;
            int m_lastUpdateMaskWithInput = 0;
            void BuildMaskFromEngineForGame();
            void BuildMaskFromEngineForMeny();
            void PrintInputMouseMovement();
            void PrintInputStructsDEBUG();

            // TODOEA går det att sätta pekare istället så man inte behöver get'a
            std::vector<int> m_musInputFromModule;
            std::vector<int> m_keyboardInputFromModule;
            int m_mouseMoveX = 0;
            int m_mouseMoveY = 0;
            int m_mouseWheelInput = 0;


            static InputHandler* m_singleton;
        };
    }
}
