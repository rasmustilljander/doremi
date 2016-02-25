#pragma once
#include <DirectXMath.h>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>


namespace Doremi
{
    namespace Core
    {
//  Every one hour we update sequences if the connection changes, why not?
// I did this because I though I had a bug with TCP first packet beeing delayed...
// But it wasn't, I only forgot to change the delayed sequence value..
#define SEQUENCE_UPDATE_TIMER 360.0f
#define SEQUENCE_TIMER_START 1000.0f
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
                : ConnectionState(ServerConnectionStateFromClient::DISCONNECTED),
                  ConnectingSocketHandle(0),
                  ConnectedSocketHandle(0),
                  ConnectingAdress(nullptr),
                  ConnectedAdress(nullptr),
                  PlayerID(0),
                  LastSequenceUpdate(SEQUENCE_TIMER_START),
                  LastResponse(0)
            {
            }
            ServerConnectionStateFromClient ConnectionState;
            SocketHandle ConnectingSocketHandle;
            SocketHandle ConnectedSocketHandle;
            DoremiEngine::Network::Adress* ConnectingAdress;
            DoremiEngine::Network::Adress* ConnectedAdress;

            PlayerID PlayerID;

            double LastSequenceUpdate;
            double LastResponse;
        };

        struct MasterConnectionFromClient
        {
            MasterConnectionStateFromClient ConnectionState;
            SocketHandle ConnectingSocketHandle;
            DoremiEngine::Network::Adress* Adress;

            double LastResponse;
        };

        struct ClientConnectionFromServer
        {
            ClientConnectionFromServer()
                : ConnectionState(ClientConnectionStateFromServer::VERSION_CHECK),
                  ConnectedSocketHandle(0),
                  PlayerID(0),
                  LastSequenceUpdate(SEQUENCE_TIMER_START),
                  LastResponse(0)
            {
            }
            ClientConnectionStateFromServer ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID PlayerID;

            double LastSequenceUpdate;
            double LastResponse;
        };

        struct MasterConnectionFromServer
        {
            MasterConnectionStateFromServer ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID PlayerID;

            double LastResponse;
        };
    }
}
