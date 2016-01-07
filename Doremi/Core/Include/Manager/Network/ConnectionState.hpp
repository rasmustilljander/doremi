#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
            Connectionstate for clients and servers
            TODOCM not sure if master servers will have the same connectionstate
        */
        enum class ConnectionState
        {
            DISCONNECTED,
            CONNECTING,
            VERSION_CHECK,
            MAP_LOADING,
            CONNECTED
        };
    }
}
