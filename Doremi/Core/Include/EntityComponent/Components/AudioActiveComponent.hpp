#pragma once
#include <map>
#include <algorithm>
#include <array>
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
            int32_t m_soundEnumToChannelID[(int32_t)(AudioCompEnum::Num_Sounds)]; // +1];
            // std::map<int, int> m_soundEnumToChannelID;
            AudioActiveComponent() { std::fill_n(m_soundEnumToChannelID, (int)(AudioCompEnum::Num_Sounds) /* + 1*/, -1); }
        };
    }
}