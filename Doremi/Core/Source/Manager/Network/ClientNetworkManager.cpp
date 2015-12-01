// Project specific
#include <Manager/Network/ClientNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <iostream> // TODOCM remove after test

namespace Doremi
{
    namespace Core
    {
        ClientNetworkManager::ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
        : Manager(p_sharedContext), m_masterConnectionState(ConnectionState::DISCONNECTED),
          m_serverConnectionState(ConnectionState::DISCONNECTED), m_nextUpdateTimer(0.0f),
          m_updateInterval(1000.0f)
        {
            // TODOCM Remove test code
            // STEP TWO: RUN THIS LINE
            size_t servSock = p_sharedContext.GetNetworkModule().ConnectToReliable(127, 0, 0, 1, 3500);

            // TODOCM Remove test code ofcourse!
            Doremi::Core::NetMessage Message = Doremi::Core::NetMessage();
            uint32_t MessageSize = sizeof(Doremi::Core::NetMessage);

            while (true)
            {
                if (p_sharedContext.GetNetworkModule().RecieveData(&Message, MessageSize, servSock))
                {
                    std::cout << Message.MessageSequence << std::endl;
                    Message.MessageSequence += 1;

                    int testInt = Message.Data[100];

                    p_sharedContext.GetNetworkModule().SendData(&Message, MessageSize, servSock);
                }
            }
            

        }

        ClientNetworkManager::~ClientNetworkManager()
        {
        }

        void ClientNetworkManager::Update(double p_dt)
        {
            m_nextUpdateTimer += p_dt;

            if(m_nextUpdateTimer >= m_updateInterval)
            {
                m_nextUpdateTimer -= m_updateInterval;
            }
        }
    }
}