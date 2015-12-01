// Project specific
#include <Manager/Network/ServerNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <iostream> // TODOCM remove after test

namespace Doremi
{
    namespace Core
    {
        ServerNetworkManager::ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
        : Manager(p_sharedContext)
        {
            // TODOCM Remove test code
            // STEP ONE: RUN THIS LINE
            size_t mySock = p_sharedContext.GetNetworkModule().CreateReliableConnection(127, 0, 0, 1, 3500);

            // TODOCM Remove test code
            // STEP THREE: RUN THIS LINE - now do whatever
            size_t clientSock = p_sharedContext.GetNetworkModule().AcceptConnection(mySock);

            // TODOCM Remove test
            Doremi::Core::NetMessage Message = Doremi::Core::NetMessage();
            Message.MessageID = 5;
            Message.MessageSequence = 1;

            int testInt = 100;
            Message.Data[100] = testInt;

            uint32_t MessageSize = sizeof(Doremi::Core::NetMessage);

            p_sharedContext.GetNetworkModule().SendData(&Message, MessageSize, clientSock);

            while (true)
            {
                if (p_sharedContext.GetNetworkModule().RecieveData(&Message, MessageSize, clientSock))
                {
                    Message.MessageSequence += 1;
                    std::cout << Message.MessageSequence << std::endl;
                    p_sharedContext.GetNetworkModule().SendData(&Message, MessageSize, clientSock);
                }
            }
            
        }

        ServerNetworkManager::~ServerNetworkManager()
        {
        }

        void ServerNetworkManager::Update(double p_dt)
        {
        }
    }
}