#pragma once
// Project specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

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
        // TODOEA Lägga in detta i en textfil.
        /**
            TODOEA doc
        */
        enum class UserCommandPlaying
        { // Key                  Code for it
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
            StartRepeatableAudioRecording = 8192, //ä
            PlayRepeatableAudioRecording = 16384, //ö
            ExitGame = 32768,
            LeftClick = 65536,
            SetFrequency0 = 131072, // 7 55
            SetFrequency500 = 262144, // 8 56
            SetFrequency1000 = 524288, // 9 57
            // TODOEA add all here or set the 1111111... bit number to ALL
            All = Jump | Forward | Backward | Left | Right | Fire | ScrollWpnUp | ScrollWpnDown, // Seen this be done so doing it here aswell! =D
        };

        // TODOEA Lägga in detta i en textfil.
        /**
            TODOEA doc
        */
        enum class UserCommandMeny
        { // Code
            LeftClick = 1, // 1
            RightClick = 2, // 3
            Enter = 4, // 13
            Up = 8, // 1073741906
            Down = 16, // 1073741905
            Left = 32, // 1073741904
            Right = 64, // 1073741903
        };

        /**
            TODOEA doc
        */
        class InputHandler
        {
        public:
            /**
                TODOEA doc
            */
            InputHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOEA doc
            */
            ~InputHandler();

            /**
                TODOEA doc
            */
            void Initialize();

            /**
                TODOEA doc
            */
            bool CheckBitMaskInputFromGame(int p_bitMask);

            /**
                TODOEA doc
            */
            bool CheckForOnePress(int p_bitMask);

            /*
                TODOCM doc
            */
            uint32_t GetInputBitMask();

            // TODOEA void ChangeKeyConfig();//Får se hur vi gör här kan göra på flera sätt.
            // Kan göra att jag har massa funktioner här eller att den menyn skickar in ett ID som
            // på vad som ska bytas så kan vi koppla det på något SKÖNT sätt ;)
            // Behöver nog ta bort old entries eller ändra dem på något sätt.

        protected:
            /**
                TODOEA doc
            */
            const DoremiEngine::Core::SharedContext& m_sharedContext;


            // TODOEA Could be a problem with meny and game inputs I DUNNO To only hav one bitmask
            /**
                TODOEA doc
            */
            uint32_t m_maskWithInput = 0;

            /**
                TODOEA doc
            */
            uint32_t m_lastUpdateMaskWithInput = 0;
        };
    }
}
