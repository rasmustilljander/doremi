#pragma once
#include <Manager/Network/ConnectionState.hpp>
#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        /**
            Struct used for a connection to client
        */
        struct Connection
        {
            ConnectionState ConnectionState;
            size_t ReliableSocketHandle;
            double LastResponse;
            bool NewConnection;
            uint8_t LastSequence;
            DirectX::XMFLOAT3 PositionOnLastSequence;
            uint32_t PlayerID;
        };
    }
}
