#pragma once
#include <map>
#include <EntityComponent/Components/AudioComponent.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */

        struct AudioActiveComponent
        {
            int m_soundEnumToChannelID[(int)(AudioCompEnum::Num_Sounds) + 1];
            // std::map<int, int> m_soundEnumToChannelID;
            AudioActiveComponent() {}
        };
    }
}