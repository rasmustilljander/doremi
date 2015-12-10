#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */
        struct VoiceRecordingComponent
        {
            size_t channelID;
            size_t soundID;
            bool active;
            bool loop;
            VoiceRecordingComponent(size_t p_channelID, size_t p_soundID, bool p_active, bool p_loop)
                : channelID(p_channelID), soundID(p_soundID), active(p_active), loop(p_loop)
            {
            }
            VoiceRecordingComponent() : channelID(-1), soundID(-1), active(false), loop(false) {}
        };
    }
}