#pragma once

// Project specific
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

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
        */
        class NetworkModule : public DoremiEngine::Core::EngineModule
        {
            public:
            /**
                Startup the Network Module, if WIN32 is specified - winsock is initialized
            */
            virtual void Startup() = 0;

            /**
                TODOCM doc
            */
            virtual void SetWorkingDirectory(const std::string& p_workingDirectory) = 0;

            /**
                Send data to a specified socket TODOCM check if this is the true way to send
               messages or if you need the adress and -from- socket
            */
            virtual void SendNeworkMessage(const NetMessage& p_message, size_t& p_sendToSocket) = 0;

            /**
                Create a unreliable socket connection
            */
            virtual void
            ConnectUnrealiable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) = 0;

            /**
                Create a socket and connects to a reliable standby socket
            */
            virtual size_t
            ConnectToReliable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) = 0;

            /**
                Create a socket in standby, which will be able to recieve reliable connections
            */
            virtual size_t
            CreateReliableConnection(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) = 0;

            /**
                Accept an incomming connection to a socket in standby(CreateReliableConnection)
            */
            virtual size_t AcceptConnection(size_t p_socketID) = 0;

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