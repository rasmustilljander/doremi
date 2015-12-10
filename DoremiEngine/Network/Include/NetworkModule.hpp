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
            Client: CreteUnreliableSocket SendUnreliableData(have to use this befor Recieve because background operation on socket)

            Then SendUnreliableData/RecieveUnreliableData
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
                Recieve data from a specified socket, returns true if successful
            */
            virtual bool RecieveReliableData(void* t_data, const uint32_t &t_dataSize, const size_t& p_recieveFromSocket) = 0;

            /**
                Send data to a specific socket with adress, returns true if successfull
            */
            virtual bool SendUnreliableData(void* p_data, const uint32_t &p_dataSize, const size_t& p_sendToSocketHandle, const Adress* p_adressToSendTo) = 0;

            /**
                Recieve data from a specific socket, Adress is fetched and returns true if successfull
            */
            virtual bool RecieveUnreliableData(void* p_data, const uint32_t &p_dataSize, const size_t& p_recieveFromSocketHandle, Adress* p_AdressOut) = 0;

            /**
                Recieve data from a specific socket and returns true if successfull
            */
            virtual bool RecieveUnreliableData(void* p_data, const uint32_t &p_dataSize, const size_t& p_recieveFromSocketHandle) = 0;

            /**
                Create a socket and connects to a reliable standby socket
            */
            virtual size_t ConnectToReliable(const Adress* p_adressToConnectTo) = 0;

            /**
                Create a socket in standby, which will be able to recieve reliable connections
            */
            virtual size_t CreateReliableConnection(const Adress* p_adressToConnectTo, uint8_t p_maxWaitingConnections) = 0;

            /**
                Accept an incomming connection to a socket in standby(CreateReliableConnection)
            */
            virtual size_t AcceptConnection(size_t p_socketID, size_t& p_outSocketID, Adress* p_adressOut) = 0;

            /**
                Creates a socket to use recieve/send, need to use SendUnreliableData first to be able to use RecieveUnreliableData from a specific
               adress
            */
            virtual size_t CreateUnreliableSocket() = 0;

            /**
                Create a socket in standby, which will be able to recieve unreliable incomming messages
            */
            virtual size_t CreateUnreliableWaitingSocket(const Adress* p_adressToConnectTo) = 0;

            /**
                Shutdown the network system, including winsock if WIN32 is specified
            */
            virtual void Shutdown() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Network::NetworkModule* (*CREATE_NETWORK_MODULE)(const DoremiEngine::Core::SharedContext&);
NETWORK_DLL_EXPORT DoremiEngine::Network::NetworkModule*
CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context);
}