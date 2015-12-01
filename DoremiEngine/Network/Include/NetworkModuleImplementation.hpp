#pragma once

// Project specific
#include <NetworkModule.hpp>

// Standard libraries
#include <Socket.hpp>
#include <string>
#include <map>

namespace DoremiEngine
{
    namespace Network
    {
        /**
            Computing the data-flow management of incomming and outcomming packages
            Creates and removes connections
        */
        class NetworkModuleImplementation : public NetworkModule
        {
            public:
            /**
                TODOCM docs
            */
            NetworkModuleImplementation();

            /**
                TODOCM docs
            */
            virtual ~NetworkModuleImplementation();

            /**
                TODOCM docs
            */
            void Startup() override;

            /**
                TODOCM docs
            */
            void SetWorkingDirectory(const std::string& p_workingDirectory) override;

            /**
                TODOCM docs
            */
            bool SendData(void* t_data, const uint32_t &t_dataSize, const size_t& p_sendToSocket) override;

            /**
                TODOCM docs
            */
            bool RecieveData(void* t_data, const uint32_t &t_dataSize, const size_t& p_recieveFromSocket) override;

            /**
                TODOCM docs
            */
            void ConnectUnrealiable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) override;

            /**
                TODOCM docs
            */
            size_t ConnectToReliable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) override;

            /**
                TODOCM docs
            */
            size_t CreateReliableConnection(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) override;

            /**
                TODOCM docs
            */
            size_t AcceptConnection(size_t p_socketID) override;

            /**
                TODOCM docs
            */
            void Shutdown() override;

            private:
            /**
                TODOCM docs
            */
            bool m_isInitialized;

            /**
                TODOCM docs
            */
            std::map<size_t, Socket*> m_SocketHandleMap; // TODOCM change way of storing socket
            // handles if possible
        };
    }
}