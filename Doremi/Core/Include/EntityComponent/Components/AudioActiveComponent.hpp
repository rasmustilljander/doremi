#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */
        struct AudioActiveComponent
        {
            size_t channelID;
            AudioActiveComponent(size_t p_channelID) { channelID = p_channelID; }
            AudioActiveComponent() : channelID(99999) {}
        };
    }
}