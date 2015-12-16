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
            INIT_SNAPSHOT,
            SNAPSHOT,
        };

        /**
            A network message will contain:
            1 byte for type of Message(Ping, ConRequest etc..)
            1023 byte for actual data
            Each message a total of 1024 Bytes.
            60 messages a second will give 0.5 Mbit/s usage
        */
        struct NetMessage
        {
            MessageID MessageID; // 1 byte
            unsigned char Data[1023]; // 1023 byte
        };
    }
}
