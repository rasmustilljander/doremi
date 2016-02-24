#pragma once
#include <DirectXMath.h>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>


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
            ServerConnectionFromClient()
                : ConnectionState(ServerConnectionStateFromClient::DISCONNECTED), PlayerID(0), LastResponse(0), NewConnection(true)
            {
            }
            ServerConnectionStateFromClient ConnectionState;
            SocketHandle ConnectingSocketHandle;
            SocketHandle ConnectedSocketHandle;
            DoremiEngine::Network::Adress* ConnectingAdress;
            DoremiEngine::Network::Adress* ConnectedAdress;

            PlayerID PlayerID;

            double LastResponse;
            bool NewConnection;
        };

        struct MasterConnectionFromClient
        {
            MasterConnectionStateFromClient ConnectionState;
            SocketHandle ConnectingSocketHandle;
            DoremiEngine::Network::Adress* Adress;

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
