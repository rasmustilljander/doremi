// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerMaster.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesServer.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Standard
#include <iostream> // TODOCM remove after test
#include <vector>
#include <algorithm>
#include <time.h>

namespace Doremi
{
    namespace Core
    {
        NetworkManagerMaster::NetworkManagerMaster(const DoremiEngine::Core::SharedContext & p_sharedContext) : Manager(p_sharedContext, "NetworkManagerMaster"), m_timeoutInterval(10) , m_maxClientMessagesPerFrame(10), m_maxServerMessagesPerFrame(10)
        {
        }
        NetworkManagerMaster::~NetworkManagerMaster()
        {
        }
        void NetworkManagerMaster::Update(double p_dt)
        {
        }
        void NetworkManagerMaster::ReceiveMessages()
        {
        }
        void NetworkManagerMaster::ReceiveMessagesClients()
        {
        }
        void NetworkManagerMaster::ReceiveMessagesServers()
        {
        }
        void NetworkManagerMaster::SendMessages()
        {
        }
        void NetworkManagerMaster::SendMessagesClients()
        {
        }
        void NetworkManagerMaster::SendMessagesServers()
        {
        }
        void NetworkManagerMaster::UpdateTimeouts(double t_dt)
        {
        }
    }
}