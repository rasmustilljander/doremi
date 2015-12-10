#pragma once
#include <Manager/Network/ConnectionState.hpp>

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
        };
    }
}
