#pragma once
#include <cstdint>

namespace Doremi
{
    namespace Core
    {
        /**
            MessageID used for identifying type of message
        */
        enum class MessageID : uint8_t
        {
            CONNECT_REQUEST,
            VERSION_CHECK,
            CONNECT,
            DISCONNECT,
            SNAPSHOT
        };

        /**
            A network message will contain:
            1 byte for type of Message(Ping, ConRequest etc..)
            255 byte for actual data
            Each message a total of 256 Bytes.
        */
        struct NetMessage
        {
            MessageID MessageID; // 1 byte
            unsigned char Data[255]; // 255 byte
        };
    }
}
