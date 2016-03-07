#pragma once
#include <cstdint>
#include <Windows.h>

namespace Doremi
{
    namespace Core
    {
        /**
            MessageID used for identifying type of message from Client
        */
        enum class SendMessageIDToServerFromClient : uint8_t
        {
            // Connecting
            CONNECTION_REQUEST,
            VERSION_CHECK,
            DISCONNECT,

            // Connected
            CONNECTED,
            LOAD_WORLD,
            IN_GAME,
        };

        /**
            MessageID used for identifying type of message from Server
        */
        enum class SendMessageIDToClientFromServer : uint8_t
        {
            // Connecting
            VERSION_CHECK,
            CONNECT,
            DISCONNECT,

            // Connected
            CONNECTED,
            LOAD_WORLD,
            IN_GAME,
        };

        enum class SendMessageIDToClientFromMaster
        {
            CONNECTED,
            DISCONNECT
        };

        enum class SendMessageIDToServerFromMaster
        {
            CONNECTED,
            DISCONNECT
        };

        enum class SendMessageIDToMasterFromClient
        {
            CONNECTION_REQUEST,
            CONNECTED,
            DISCONNECT
        };

        enum class SendMessageIDToMasterFromServer
        {
            CONNECTION_REQUEST,
            CONNECTED,
            DISCONNECT
        };


/**
    A network message will contain:
    1 byte for type of MessageID( ConRequest, Version Check etc..)
    Data varies between Messages,(Connecting, Connected)
    The average connection speed found on wikipedia is 5.1 Mb/s, from wikipedia &
   http://www.statista.com/statistics/204952/average-internet-connection-speed-by-country/
    1.8 Mb/s from http://royal.pingdom.com/2010/11/12/real-connection-speeds-for-internet-users-across-the-world/
    Living in a country with the top bandwiths in the world might make it easy to assume everyone got bandwith to spare worldwide,
    Looking at graphcs this is not the case therefor we limit our bandiwht speed to 1 Mbit/s
    This gives room for additional voice applications in the background to run.
    60 Messages are sent per second on the client which gives connected - 1.0 Mbit/s upstream & downstream
    Server side connectionwise, will be limited here, needing a stream of 2.0 Mbit/s up/down minimum, increasing linear by player
*/

#define NET_MESSAGE_BUFFER_DATA_SIZE 2047
#define NET_MESSAGE_CONNECTING_DATA_SIZE 1023 // TODOCM change
#define NET_MESSAGE_CONNECTED_DATA_SIZE NET_MESSAGE_BUFFER_DATA_SIZE
#define NET_MESSAGE_SMALL 511

        /**
            Message sent as buffer from Network Module
        */
        struct NetMessageBuffer
        {
            NetMessageBuffer() : MessageID(0) { ZeroMemory(Data, NET_MESSAGE_BUFFER_DATA_SIZE); }
            uint8_t MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_BUFFER_DATA_SIZE]; // 2047 byte
        };

        /**
            Server - Client messages
        */

        struct NetMessageServerClientConnectingFromClient
        {
            NetMessageServerClientConnectingFromClient() : MessageID(SendMessageIDToServerFromClient::CONNECTION_REQUEST)
            {
                ZeroMemory(Data, NET_MESSAGE_CONNECTING_DATA_SIZE);
            }
            SendMessageIDToServerFromClient MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_CONNECTING_DATA_SIZE]; // 1023 byte
        };

        struct NetMessageServerClientConnectedFromClient
        {
            NetMessageServerClientConnectedFromClient() : MessageID(SendMessageIDToServerFromClient::CONNECTION_REQUEST)
            {
                ZeroMemory(Data, NET_MESSAGE_CONNECTED_DATA_SIZE);
            }
            SendMessageIDToServerFromClient MessageID;
            unsigned char Data[NET_MESSAGE_CONNECTED_DATA_SIZE]; // 2047 byte
        };

        struct NetMessageServerClientConnectingFromServer
        {
            NetMessageServerClientConnectingFromServer() : MessageID(SendMessageIDToClientFromServer::VERSION_CHECK)
            {
                ZeroMemory(Data, NET_MESSAGE_CONNECTING_DATA_SIZE);
            }
            SendMessageIDToClientFromServer MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_CONNECTING_DATA_SIZE]; // 1023 byte
        };

        struct NetMessageServerClientConnectedFromServer
        {
            NetMessageServerClientConnectedFromServer() : MessageID(SendMessageIDToClientFromServer::VERSION_CHECK)
            {
                ZeroMemory(Data, NET_MESSAGE_CONNECTED_DATA_SIZE);
            }
            SendMessageIDToClientFromServer MessageID;
            unsigned char Data[NET_MESSAGE_CONNECTED_DATA_SIZE]; // 2047 byte
        };

        /**
            Master - Client messages
        */

        struct NetMessageMasterClientFromMaster
        {
            NetMessageMasterClientFromMaster() : MessageID(SendMessageIDToClientFromMaster::CONNECTED)
            {
                ZeroMemory(Data, NET_MESSAGE_CONNECTED_DATA_SIZE);
            }
            SendMessageIDToClientFromMaster MessageID;
            unsigned char Data[NET_MESSAGE_CONNECTED_DATA_SIZE]; // 2047 byte
        };

        struct NetMessageMasterClientFromClient
        {
            NetMessageMasterClientFromClient() : MessageID(SendMessageIDToMasterFromClient::CONNECTED) { ZeroMemory(Data, NET_MESSAGE_SMALL); }
            SendMessageIDToMasterFromClient MessageID;
            unsigned char Data[NET_MESSAGE_SMALL]; // 2047 byte
        };

        /**
            Master - Server messages
        */
        struct NetMessageMasterServerFromMaster
        {
            NetMessageMasterServerFromMaster() : MessageID(SendMessageIDToServerFromMaster::CONNECTED) { ZeroMemory(Data, NET_MESSAGE_SMALL); }
            SendMessageIDToServerFromMaster MessageID;
            unsigned char Data[NET_MESSAGE_SMALL]; // 2047 byte
        };

        struct NetMessageMasterServerFromServer
        {
            NetMessageMasterServerFromServer() : MessageID(SendMessageIDToMasterFromServer::CONNECTION_REQUEST)
            {
                ZeroMemory(Data, NET_MESSAGE_SMALL);
            }
            SendMessageIDToMasterFromServer MessageID;
            unsigned char Data[NET_MESSAGE_SMALL]; // 2047 byte
        };
    }
}
