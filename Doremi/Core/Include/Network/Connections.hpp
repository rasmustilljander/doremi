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
            CONNECTED,
        };

        enum class ClientConnectionStateFromMaster
        {
            CONNECTED,
        };

        enum class ServerConnectionStateFromMaster
        {
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
                  MyPlayerID(0),
                  LastSequenceUpdate(SEQUENCE_TIMER_START),
                  LastResponse(0)
            {
            }
            ServerConnectionStateFromClient ConnectionState;
            SocketHandle ConnectingSocketHandle;
            SocketHandle ConnectedSocketHandle;
            DoremiEngine::Network::Adress* ConnectingAdress;
            DoremiEngine::Network::Adress* ConnectedAdress;

            PlayerID MyPlayerID;

            double LastSequenceUpdate;
            double LastResponse;
        };

        struct MasterConnectionFromClient
        {
            MasterConnectionFromClient()
                : ConnectionState(MasterConnectionStateFromClient::CONNECTING), ConnectedSocketHandle(0), Adress(nullptr), LastResponse(0)
            {
            }
            MasterConnectionStateFromClient ConnectionState;
            SocketHandle ConnectedSocketHandle;
            DoremiEngine::Network::Adress* Adress;

            double LastResponse;
        };

        struct ClientConnectionFromServer
        {
            ClientConnectionFromServer()
                : ConnectionState(ClientConnectionStateFromServer::VERSION_CHECK),
                  ConnectedSocketHandle(0),
                  MyPlayerID(0),
                  LastSequenceUpdate(SEQUENCE_TIMER_START),
                  LastResponse(0)
            {
            }
            ClientConnectionStateFromServer ConnectionState;
            SocketHandle ConnectedSocketHandle;
            PlayerID MyPlayerID;

            double LastSequenceUpdate;
            double LastResponse;
        };

        struct MasterConnectionFromServer
        {
            MasterConnectionFromServer()
                : ConnectionState(MasterConnectionStateFromServer::CONNECTING), ConnectedSocketHandle(0), Adress(nullptr), LastResponse(0.0f), AckSequence(0)
            {
            }
            MasterConnectionStateFromServer ConnectionState;
            SocketHandle ConnectedSocketHandle;
            DoremiEngine::Network::Adress* Adress;

            double LastResponse;
            uint8_t AckSequence;
        };

        struct ClientConnectionFromMaster
        {
            ClientConnectionFromMaster() : ConnectionState(ClientConnectionStateFromMaster::CONNECTED), LastResponse(0.0f) {}
            ClientConnectionStateFromMaster ConnectionState;

            double LastResponse;
        };

        struct ServerConnectionFromMaster
        {
            ServerConnectionFromMaster()
                : ConnectionState(ServerConnectionStateFromMaster::CONNECTED),
                  LastResponse(0.0f),
                  CurrentSequence(0),
                  SequenceToCheck(0),
                  SequenceLastResponse(0.0f),
                  ServerName(""),
                  ServerState(0),
                  Map(0),
                  Ping(0),
                  CurrentPlayers(0),
                  MaxPlayers(0),
                  ConnectingPort(0),
                  IP_a(0),
                  IP_b(0),
                  IP_c(0),
                  IP_d(0)
            {
            }
            ServerConnectionStateFromMaster ConnectionState;
            double LastResponse;

            uint8_t CurrentSequence;
            uint8_t SequenceToCheck;
            double SequenceLastResponse;

            std::string ServerName;
            uint8_t ServerState;
            uint8_t Map;
            uint16_t Ping;

            uint8_t CurrentPlayers;
            uint8_t MaxPlayers;

            uint16_t ConnectingPort;
            uint8_t IP_a;
            uint8_t IP_b;
            uint8_t IP_c;
            uint8_t IP_d;
        };
    }
}
