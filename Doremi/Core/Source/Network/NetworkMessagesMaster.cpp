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

// Handler
#include <Doremi/Core/Include/NetworkEventSender.hpp>

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
    }
}