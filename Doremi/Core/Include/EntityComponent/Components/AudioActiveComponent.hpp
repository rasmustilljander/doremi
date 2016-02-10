#pragma once
#include <map>
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */

        struct AudioActiveComponent
        {
            std::map<int, int> m_soundEnumToChannelID;
            AudioActiveComponent() {}
        };
    }
}