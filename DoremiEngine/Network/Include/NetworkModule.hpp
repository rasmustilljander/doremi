#pragma once

// Project specific
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>

#if defined(_WINDLL)
#define NETWORK_DLL_EXPORT __declspec(dllexport)
#else
#define NETWORK_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Network
    {
        struct NetMessage;

        /**
            Computing the data-flow management of incomming and outcomming packages
            Creates and removes connections

            Reliable flow
            Server: CreateReliableConnection -> AcceptConnection
            Client: ConnectToReliable

            Then send/recv

            Unreliable flow
            Server: CreateUnreliableWaitingSocket
            Client: CreteUnreliableSocket SendUnreliableData(have to use this befor Receive because background operation on socket)

            Then SendUnreliableData/ReceiveUnreliableData
        */
        class NetworkModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
                Startup the Network Module, if WIN32 is specified - winsock is initialized
            */
            virtual void Startup() = 0;

            /**
                Create adress
            */
            virtual Adress* CreateAdress() = 0;

            /**
                Create copy of adress
            */
            virtual Adress* CreateAdress(const Adress& m_adress) = 0;

            /**
                Create adress with Any input IP and set port
            */
            virtual Adress* CreateAdress(uint16_t p_port) = 0;

            /**
                Create adress class with IP and Port as p_a.p_b.p_c.p_d as 255.255.255.255
            */
            virtual Adress* CreateAdress(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) = 0;

            /**
                Send data to a specified socket, returns true if successful
            */
            virtual bool SendReliableData(void* t_data, const uint32_t& t_dataSize, const size_t& p_sendToSocket) = 0;

            /**
                Receive data from a specified socket, returns true if successful
            */
            virtual bool ReceiveReliableData(void* t_data, const uint32_t& t_dataSize, const size_t& p_ReceiveFromSocket, uint32_t& p_dataSizeReceived) = 0;

            /**
                Send data to a specific socket with adress, returns true if successfull
            */
            virtual bool SendUnreliableData(void* p_data, const uint32_t& p_dataSize, const size_t& p_sendToSocketHandle, const Adress* p_adressToSendTo) = 0;

            /**
                Receive data from a specific socket, Adress is fetched and returns true if successfull
            */
            virtual bool ReceiveUnreliableData(void* p_data, const uint32_t& p_dataSize, const size_t& p_ReceiveFromSocketHandle, Adress* p_AdressOut,
                                               uint32_t& p_dataSizeReceived) = 0;

            /**
                Receive data from a specific socket and returns true if successfull
            */
            virtual bool ReceiveUnreliableData(void* p_data, const uint32_t& p_dataSize, const size_t& p_ReceiveFromSocketHandle, uint32_t& p_dataSizeReceived) = 0;

            /**
                Create a socket and connects to a reliable standby socket
            */
            virtual bool ConnectToReliable(const Adress* p_adressToConnectTo, size_t& o_socketHandle) = 0;

            /**
                Create a socket in standby, which will be able to Receive reliable connections
            */
            virtual size_t CreateReliableConnection(const Adress* p_adressToConnectTo, uint8_t p_maxWaitingConnections) = 0;

            /**
                Accept an incomming connection to a socket in standby(CreateReliableConnection)
            */
            virtual bool AcceptConnection(size_t p_socketID, size_t& p_outSocketID, Adress* p_adressOut) = 0;

            /**
                Creates a socket to use Receive/send, need to use SendUnreliableData first to be able to use ReceiveUnreliableData from a specific
               adress
            */
            virtual size_t CreateUnreliableSocket() = 0;

            /**
                Create a socket in standby, which will be able to Receive unreliable incomming messages
            */
            virtual size_t CreateUnreliableWaitingSocket(const Adress* p_adressToConnectTo) = 0;

            /**
                TODOCM doc
            */
            virtual void DeleteSocket(size_t p_socketID) = 0;

            /**
                Shutdown the network system, including winsock if WIN32 is specified
            */
            virtual void Shutdown() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Network::NetworkModule* (*CREATE_NETWORK_MODULE)(const DoremiEngine::Core::SharedContext&);
NETWORK_DLL_EXPORT DoremiEngine::Network::NetworkModule* CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context);
}