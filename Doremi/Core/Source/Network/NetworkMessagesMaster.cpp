#include <Doremi/Core/Include/Network/NetworkMessagesMaster.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsMaster.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>

// Network
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>


#include <Doremi/Core/Include/SequenceMath.hpp>

#include <iostream> // TODOCM remove this debug

namespace Doremi
{
    namespace Core
    {
        NetworkMessagesMaster* NetworkMessagesMaster::m_singleton = nullptr;

        NetworkMessagesMaster* NetworkMessagesMaster::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                std::runtime_error("Called GetInstance before StartupNetworkMessagesServer.");
            }
            return m_singleton;
        }

        void NetworkMessagesMaster::StartupNetworkMessagesMaster(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if (m_singleton != nullptr)
            {
                std::runtime_error("Called StartupNetworkMessagesServer multiple times.");
            }
            m_singleton = new NetworkMessagesMaster(p_sharedContext);
        }

        NetworkMessagesMaster::NetworkMessagesMaster(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        NetworkMessagesMaster::~NetworkMessagesMaster() {}

        /**
            Receive Client
        */

        void NetworkMessagesMaster::ReceiveConnectionRequestClient(NetMessageMasterClientFromClient& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromMaster* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if (!NetworkConnectionsMaster::GetInstance()->AdressWithPortExistClients(p_adress, t_connection))
            {
                NetworkConnectionsMaster::GetInstance()->CreateNewConnectionClients(p_adress);

                // Send connected message
                SendConnectedClient(p_adress);
            }
        }

        void NetworkMessagesMaster::ReceiveConnectedClient(NetMessageMasterClientFromClient& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromMaster* t_connection;
            if(NetworkConnectionsMaster::GetInstance()->AdressWithPortExistClients(p_adress, t_connection))
            {
                t_connection->LastResponse = 0;
                SendConnectedClient(p_adress);
            }
        }

        void NetworkMessagesMaster::ReceiveDisconnectClient(NetMessageMasterClientFromClient& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromMaster* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if (NetworkConnectionsMaster::GetInstance()->AdressWithPortExistClients(p_adress, t_connection))
            {
                std::cout << "Received disconnect" << std::endl;
                t_connection->LastResponse = 10000000; // Hard coded to disconnect
            }
        }

        /**
            Receive Server
        */

        void NetworkMessagesMaster::ReceiveConnectionRequestServer(NetMessageMasterServerFromServer& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ServerConnectionFromMaster* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(!NetworkConnectionsMaster::GetInstance()->AdressWithPortExistServers(p_adress, t_connection))
            {
                t_connection = NetworkConnectionsMaster::GetInstance()->CreateNewConnectionServers(p_adress);

                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));


                t_connection->ServerName = t_streamer.ReadStringShort(); // 20 bytes
                t_connection->ServerName.resize(19);
                t_connection->ServerState = t_streamer.ReadUnsignedInt8(); // 1 byte
                t_connection->Map = t_streamer.ReadUnsignedInt8(); // 1 byte

                t_connection->CurrentPlayers = t_streamer.ReadUnsignedInt8(); // 1 byte
                t_connection->MaxPlayers = t_streamer.ReadUnsignedInt8(); // 1 byte

                t_connection->ConnectingPort = t_streamer.ReadUnsignedInt16(); // 2 byte

                // Get IP from adress
                t_connection->IP_a = p_adress.GetIP_A();
                t_connection->IP_b = p_adress.GetIP_B();
                t_connection->IP_c = p_adress.GetIP_C();
                t_connection->IP_d = p_adress.GetIP_D();

                // Send connected message
                SendConnectedServer(p_adress, t_connection->CurrentSequence);
            }
        }

        void NetworkMessagesMaster::ReceiveConnectedServer(NetMessageMasterServerFromServer& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ServerConnectionFromMaster* t_connection;
            if (NetworkConnectionsMaster::GetInstance()->AdressWithPortExistServers(p_adress, t_connection))
            {
                // Increment sequence
                t_connection->CurrentSequence++;

                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));

                uint8_t SequenceIncomming = t_streamer.ReadUnsignedInt8();

                // If it is sequence we check ping vs
                if(SequenceIncomming == t_connection->SequenceToCheck)
                {
                    // Ping should be round trip time
                    t_connection->Ping = static_cast<uint16_t>(t_connection->SequenceLastResponse * 1000.0f);
                    // TODOCM this isn't the real ping

                    // New sequence
                    t_connection->SequenceToCheck = t_connection->CurrentSequence;
                    t_connection->SequenceLastResponse = 0;
                }
                else if (sequence_more_recent(SequenceIncomming, t_connection->SequenceToCheck, 255))
                {
                    // If the packet got lost(probobly), choose a new sequence
                    t_connection->SequenceToCheck = t_connection->CurrentSequence;
                    t_connection->SequenceLastResponse = 0;
                }

                // Read current state
                t_connection->ServerState = t_streamer.ReadUnsignedInt8(); // 1 byte

                // Read current number of players
                t_connection->CurrentPlayers = t_streamer.ReadUnsignedInt8(); // 1 byte


                t_connection->LastResponse = 0;

                SendConnectedServer(p_adress, t_connection->CurrentSequence);
            }
        }

        void NetworkMessagesMaster::ReceiveDisconnectServer(NetMessageMasterServerFromServer& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ServerConnectionFromMaster* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if (NetworkConnectionsMaster::GetInstance()->AdressWithPortExistServers(p_adress, t_connection))
            {
                std::cout << "Received disconnect" << std::endl;
                t_connection->LastResponse = 10000000; // Hard coded to disconnect
            }
        }

        /**
            Send Client
        */

        void NetworkMessagesMaster::SendConnectedClient(DoremiEngine::Network::Adress& p_adress)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkConnectionsMaster* t_connections = NetworkConnectionsMaster::GetInstance();
            auto& t_serverConnections = t_connections->GetServerConnections();

            // Create connected message
            NetMessageMasterClientFromMaster t_newMessage = NetMessageMasterClientFromMaster();
            t_newMessage.MessageID = SendMessageIDToClientFromMaster::CONNECTED;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* p_bufferPointer = t_newMessage.Data;
            t_streamer.SetTargetBuffer(p_bufferPointer, sizeof(t_newMessage.Data));

            // Counter for bytes written
            uint32_t t_bytesWritten = 0;

            // Counter for servers to send
            uint8_t t_serverCounter = 0;

            // Move forward for number of servers
            t_streamer.SetReadWritePosition(sizeof(uint8_t));

            for (auto& t_connection : t_serverConnections)
            {
                // If we have enough memory to write
                if (sizeof(t_newMessage.Data) - t_bytesWritten >= 32)
                {
                   t_streamer.WriteStringShort(t_connection.second->ServerName); // 20 bytes
                   t_streamer.WriteUnsignedInt8(t_connection.second->ServerState); // 1 byte
                   t_streamer.WriteUnsignedInt8(t_connection.second->Map); // 1 byte
                   t_streamer.WriteUnsignedInt16(t_connection.second->Ping); // 2 bytes

                   t_streamer.WriteUnsignedInt8(t_connection.second->CurrentPlayers); // 1 byte
                   t_streamer.WriteUnsignedInt8(t_connection.second->MaxPlayers); // 1 byte

                   t_streamer.WriteUnsignedInt16(t_connection.second->ConnectingPort); // 2 byte
                   t_streamer.WriteUnsignedInt8(t_connection.second->IP_a); // 1 byte
                   t_streamer.WriteUnsignedInt8(t_connection.second->IP_b); // 1 byte
                   t_streamer.WriteUnsignedInt8(t_connection.second->IP_c); // 1 byte
                   t_streamer.WriteUnsignedInt8(t_connection.second->IP_d); // 1 byte

                   // Add written to accumulator
                   t_bytesWritten += 32;

                   // Increment server written
                   ++t_serverCounter;
                }
            }

            // Write number of servers
            t_streamer.SetReadWritePosition(0);
            t_streamer.WriteUnsignedInt8(t_serverCounter);

            // Send the message
            t_networkModule.SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->GetSocketHandleForClients(), &p_adress);
        }

        void NetworkMessagesMaster::SendDisconnectClient(DoremiEngine::Network::Adress& p_adress)
        {
            // Create disconnection message
            NetMessageMasterClientFromMaster t_newMessage = NetMessageMasterClientFromMaster();
            t_newMessage.MessageID = SendMessageIDToClientFromMaster::DISCONNECT;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                NetworkConnectionsMaster::GetInstance()->GetSocketHandleForClients(), &p_adress);
        }

        /**
            Send Server
        */

        void NetworkMessagesMaster::SendConnectedServer(DoremiEngine::Network::Adress& p_adress, uint8_t p_sequence)
        {
            // Create disconnection message
            NetMessageMasterServerFromMaster t_newMessage = NetMessageMasterServerFromMaster();
            t_newMessage.MessageID = SendMessageIDToServerFromMaster::CONNECTED;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* p_bufferPointer = t_newMessage.Data;
            t_streamer.SetTargetBuffer(p_bufferPointer, sizeof(t_newMessage.Data));

            // Write sequence
            t_streamer.WriteUnsignedInt8(p_sequence);

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                                                                  NetworkConnectionsMaster::GetInstance()->GetSocketHandleForServers(), &p_adress);
        }

        void NetworkMessagesMaster::SendDisconnectServer(DoremiEngine::Network::Adress& p_adress)
        {
            // Create disconnection message
            NetMessageMasterServerFromMaster t_newMessage = NetMessageMasterServerFromMaster();
            t_newMessage.MessageID = SendMessageIDToServerFromMaster::DISCONNECT;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                NetworkConnectionsMaster::GetInstance()->GetSocketHandleForServers(), &p_adress);
        }
    }
}