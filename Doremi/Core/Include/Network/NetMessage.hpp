#pragma once
#include <cstdint>
#include <Windows.h>

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
            CONNECTED,
            DISCONNECT,
            LOAD_WORLD,
            INIT_SNAPSHOT,
            SNAPSHOT,
            INPUT,
        };

/**
    A network message will contain:
    1 byte for type of Message(Ping, ConRequest etc..)
    1023 byte for actual data
    Each message a total of 1024 Bytes.
    60 messages a second will give 0.5 Mbit/s usage
*/

#define NET_MESSAGE_DATA_SIZE 1024

        struct NetMessage
        {
            NetMessage() { ZeroMemory(Data, NET_MESSAGE_DATA_SIZE); }
            MessageID MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_DATA_SIZE]; // 1023 byte
        };
    }
}
