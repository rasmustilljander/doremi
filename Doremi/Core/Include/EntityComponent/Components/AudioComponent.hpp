#pragma once
#include <map>
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */
        enum class AudioCompEnum : int
        {
            Jump,
            DebugSound,
        };
        struct AudioComponent
        {
            std::map<AudioCompEnum, int> m_enumToSoundID;
            AudioComponent() {}
            
        };
    }
}