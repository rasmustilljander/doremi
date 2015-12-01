#pragma once

namespace Doremi
{
    namespace Core
    {
        /**
            A network message will contain:
            2 byte sequence(order of messages sent)
            2 byte for type of Message(Ping, ConRequest etc..)
            252 byte for actual data
            Each message a total of 256 Bytes.
        */
        struct NetMessage
        {
            short MessageSequence; // 2 byte
            short MessageID; // 2 byte
            unsigned char Data[252]; // 252 byte
        };
    }
}
