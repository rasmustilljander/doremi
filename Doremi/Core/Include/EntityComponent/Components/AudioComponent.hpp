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
        enum class AudioCompEnum : int
        {
            Jump,
            DebugSound,

            Num_Sounds,
        };
        struct AudioComponent
        {
            // std::map<AudioCompEnum, int> m_enumToSoundID;

            int m_enumToSoundID[(int)(AudioCompEnum::Num_Sounds) + 1];
            // int mySoundID = sounds[(int)AudioCompEnum::Jump];
            AudioComponent() {}
        };
    }
}