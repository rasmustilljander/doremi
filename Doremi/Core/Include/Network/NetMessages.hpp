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
        enum class SendMessageIDFromClient : uint8_t
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
        enum class SendMessageIDFromServer : uint8_t
        {
            // Connecting
            VERSION_CHECK,
            CONNECT,
            DISCONNECT,

            // Connected
            CONNECTED,
            LOAD_WORLD,
            INIT_IN_GAME,
            IN_GAME,
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

        /**
            Message sent as buffer from Network Module
        */
        struct NetMessageBuffer
        {
            NetMessageBuffer() { ZeroMemory(Data, NET_MESSAGE_BUFFER_DATA_SIZE); }
            uint8_t MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_BUFFER_DATA_SIZE]; // 1023 byte
        };

        struct NetMessageConnectingFromClient
        {
            NetMessageConnectingFromClient() { ZeroMemory(Data, NET_MESSAGE_CONNECTING_DATA_SIZE); }
            SendMessageIDFromClient MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_CONNECTING_DATA_SIZE]; // 1023 byte
        };

        struct NetMessageConnectedFromClient
        {
            NetMessageConnectedFromClient() { ZeroMemory(Data, NET_MESSAGE_CONNECTED_DATA_SIZE); }
            SendMessageIDFromClient MessageID;
            unsigned char Data[NET_MESSAGE_CONNECTED_DATA_SIZE];
        };

        struct NetMessageConnectingFromServer
        {
            NetMessageConnectingFromServer() { ZeroMemory(Data, NET_MESSAGE_CONNECTING_DATA_SIZE); }
            SendMessageIDFromServer MessageID; // 1 byte
            unsigned char Data[NET_MESSAGE_CONNECTING_DATA_SIZE]; // 1023 byte
        };

        struct NetMessageConnectedFromServer
        {
            NetMessageConnectedFromServer() { ZeroMemory(Data, NET_MESSAGE_CONNECTED_DATA_SIZE); }
            SendMessageIDFromServer MessageID;
            unsigned char Data[NET_MESSAGE_CONNECTED_DATA_SIZE];
        };
    }
}
