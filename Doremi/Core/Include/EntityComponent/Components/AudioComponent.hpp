#pragma once
#include <map>
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */
        // If you add someone,
        enum class AudioCompEnum : int32_t
        {
            Jump,
            Death,
            DebugSound,

            Num_Sounds,
        };
        struct AudioComponent
        {
            // std::map<AudioCompEnum, int> m_enumToSoundID;

            int32_t m_enumToSoundID[(int32_t)(AudioCompEnum::Num_Sounds)]; // +1]; //Todo maybe bugs out
            // int mySoundID = sounds[(int)AudioCompEnum::Jump];
            AudioComponent()
            {
                for(int32_t i = 0; i < (int32_t)AudioCompEnum::Num_Sounds; i++)
                {
                    m_enumToSoundID[i] = (int32_t)AudioCompEnum::DebugSound;
                }
            }
        };
    }
}