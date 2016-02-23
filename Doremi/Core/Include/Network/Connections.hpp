#pragma once
#include <DirectXMath.h>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

#define CONNECTING_UPDATE_INTERVAL 0.017f
#define CONNECTED_UPDATE_INTERVAL 0.033f

namespace Doremi
{
    namespace Core
    {
        /*
            Connectionstate for clients and servers
            TODOCM not sure if master servers will have the same connectionstate
        **/
        enum class ServerConnectionStateFromClient
        {
            DISCONNECTED,
            CONNECTING,
            VERSION_CHECK,
            CONNECTED,
            LOAD_WORLD,
            IN_GAME
        };

        enum class MasterConnectionStateFromClient
        {
            DISCONNECTED,
            CONNECTING,
            VERSION_CHECK,
            CONNECTED,
        };

        enum class ClientConnectionStateFromServer
        {
            VERSION_CHECK,
            CONNECT,
            CONNECTED,
            LOAD_WORLD,
            IN_GAME
        };

        enum class MasterConnectionStateFromServer
        {
            DISCONNECTED,
            CONNECTING,
            VERSION_CHECK,
            CONNECTED,
        };

        typedef size_t SocketHandle;

        /**
            Struct used for a connection to client
        */
        struct ServerConnectionFromClient
        {
            ServerConnectionStateFromClient ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID PlayerID;

            double UpdateInterval;
            double LastResponse;
            bool NewConnection;
        };

        struct MasterConnectionFromClient
        {
            MasterConnectionStateFromClient ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID PlayerID;

            double LastResponse;
            bool NewConnection;
        };

        struct ClientConnectionFromServer
        {
            ClientConnectionFromServer() {}
            ClientConnectionStateFromServer ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID PlayerID;

            double LastResponse;
            bool NewConnection;
        };

        struct MasterConnectionFromServer
        {
            MasterConnectionStateFromServer ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID PlayerID;

            double LastResponse;
            bool NewConnection;
        };
    }
}
