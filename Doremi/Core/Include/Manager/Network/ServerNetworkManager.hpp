#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

// Standard libraries
#include <map>
#include <list>

namespace DoremiEngine
{
    namespace Network
    {
        class Adress;
    }
}

namespace Doremi
{
    namespace Core
    {
        struct Connection;
        struct NetMessage;

        /**
            Manager class for Server, contains the logic for the connection pattern
        */
        class ServerNetworkManager : public Manager
        {
            public:
            /**
                TODOCM doc
            */
            ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            virtual ~ServerNetworkManager();

            /**
                TODOCM doc
            */
            void Update(double p_dt) override;

            private:
            /**
                Recieves reliable and unreliable messages
            */
            void RecieveMessages(double p_dt);

            /**
                Checks if adress exists in connections ( both port & IP)
            */
            bool AdressExist(const DoremiEngine::Network::Adress& m_Adress, Connection*& m_connection);

            /**
                Check for incomming unreliable messages
            */
            void RecieveUnreliableMessages();

            /**
                Check for incomming reliable messages
            */
            void RecieveReliableMessages();

            /**
                Recieve a connection request message
            */
            void RecieveConnectionRequest(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_Adress);

            /**
                Recieves a version check message
            */
            void RecieveVersionCheck(NetMessage& m_message, const DoremiEngine::Network::Adress& m_Adress);

            /**
                Recieves a disconnect message
            */
            void RecieveDisconnect(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress);

            /**
                Send reliable messages to all connected clients
            */
            void SendMessages(double p_dt);

            /**
                Send disconnect message to adress
            */
            void SendDisconnect(const DoremiEngine::Network::Adress& m_adress, std::string p_outString);

            /**
                Send version check message
            */
            void SendVersionCheck(const DoremiEngine::Network::Adress& m_adress);

            /**
                Send connect message
            */
            void SendConnect(const Connection* connection, const DoremiEngine::Network::Adress& m_adress);

            /**
                TODOCM doc
            */
            void SendConnected(Connection* p_connection);

            /**
                TODOCM doc
            */
            void SendLoadWorld(Connection* p_connection);

            /**
                TODOCM doc
            */
            void SendInGame(NetMessage& p_message, Connection* p_connection);

            /**
                Check for reliable connections to accept
            */
            void CheckForConnections();

            /**
                Update timers for clients, and check if anyone got timeouted
            */
            void UpdateTimeouts(double t_dt);

            /**
                TODOCM doc
            */
            void CreateSnapshot(unsigned char* p_buffer, uint32_t p_bufferSize, Connection* p_connection);

            /**
                TODOCM doc
            */
            void RecieveInputMessage(NetMessage& p_message, Connection* p_connection);

            /**
                Timer for next send
            */
            double m_nextUpdateTimer;

            /**
                Time interval to next send
            */
            double m_updateInterval;

            /**
                Time interval for timeouts
            */
            double m_timeoutInterval;

            /**
                Socket for incomming unreliable messages
            */
            size_t m_unreliableSocketHandle;

            /**
                Socket for incomming reliable messages
            */
            size_t m_reliableSocketHandle;

            /**
                Max number of connections
            */
            uint8_t m_maxConnection;

            /**
                TODOCM doc
            */
            uint8_t m_nextSnapshotSequence;
            /**
            
                test
            */
            uint32_t counter;

            /**
                All the client connections mapped to adresses
            */
            std::map<DoremiEngine::Network::Adress*, Connection*> m_connections;

            /**
                TODOCM doc
                TODOCM move to external place?
            */
            std::list<uint32_t> m_SavedPlayerIDs;
        };
    }
}
