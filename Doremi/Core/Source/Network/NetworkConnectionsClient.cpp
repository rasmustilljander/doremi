#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>
#include <DoremiEngine\Network\Include\NetworkModule.hpp>

#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkConnectionsClient* NetworkConnectionsClient::m_singleton = nullptr;

        NetworkConnectionsClient* NetworkConnectionsClient::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("Called GetInstance before StartupNetworkConnectionsClient.");
            }
            return m_singleton;
        }

        void NetworkConnectionsClient::StartupNetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("Called StartupNetworkConnectionsClient multiple times.");
            }
            m_singleton = new NetworkConnectionsClient(p_sharedContext);
        }

        NetworkConnectionsClient::NetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {
            // Create adress and socket for master
            LoadMasterFromConfigFile(p_sharedContext);

            // Create adress and socket for server and load playerID
            LoadServerFromConfigFile(p_sharedContext);
        }

        void NetworkConnectionsClient::LoadMasterFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();
            DoremiEngine::Configuration::ConfiguartionInfo t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();

            // Get IP from config file
            std::string t_IPString = t_configInfo.IPToMaster;

            // Create pointer to use
            char* t_IPCharPointer = new char[t_IPString.size() + 1];

            // Copy string to pointer
            t_IPString.copy(t_IPCharPointer, t_IPString.size());

            // Create int array to hold IP values
            int* t_IPArray = new int[4];

            // Pointer to part of string to check
            char* t_ToCheck;
            t_ToCheck = strtok(t_IPCharPointer, ".");

            size_t t_counter = 0;
            while(t_ToCheck != NULL && t_counter < 4)
            {
                // Save string part as int to array
                t_IPArray[t_counter] = std::stoi(std::string(t_ToCheck));
                t_ToCheck = strtok(NULL, ".");
                t_counter++;
            }

            // If we got values for all int slots we got an actual IP
            if(t_counter == 4)
            {
                // TODOCM Read port ? as well, change to master as well probobly
                m_masterConnection.Adress = t_networkModule.CreateAdress(t_IPArray[0], t_IPArray[1], t_IPArray[2], t_IPArray[3],
                                                                         t_configInfo.PortMasterClient); // TODOCM remove test code
            }
            else
            {
                m_masterConnection.Adress = t_networkModule.CreateAdress(127, 0, 0, 1, t_configInfo.PortMasterClient);
            }

            // Create new socket for unreliable
            m_masterConnection.ConnectedSocketHandle = t_networkModule.CreateUnreliableSocket();

            delete[] t_IPCharPointer;
            delete[] t_IPArray;
        }

        void NetworkConnectionsClient::LoadServerFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();
            DoremiEngine::Configuration::ConfiguartionInfo t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();

            // Get last playerID
            m_serverConnection.MyPlayerID = t_configInfo.LastServerPlayerID;

            // Get IP from config file
            std::string t_IPString = t_configInfo.IPToServer;

            // Create pointer to use
            char* t_IPCharPointer = new char[t_IPString.size() + 1];

            // Copy string to pointer
            t_IPString.copy(t_IPCharPointer, t_IPString.size());

            // Create int array to hold IP values
            int* t_IPArray = new int[4];

            // Pointer to part of string to check
            char* t_ToCheck;
            t_ToCheck = strtok(t_IPCharPointer, ".");

            size_t t_counter = 0;
            while(t_ToCheck != NULL && t_counter < 4)
            {
                // Save string part as int to array
                t_IPArray[t_counter] = std::stoi(std::string(t_ToCheck));
                t_ToCheck = strtok(NULL, ".");
                t_counter++;
            }

            // If we got values for all int slots we got an actual IP
            if(t_counter == 4)
            {
                // TODOCM Read port ? as well, change to master as well probobly
                m_serverConnection.ConnectingAdress = t_networkModule.CreateAdress(t_IPArray[0], t_IPArray[1], t_IPArray[2], t_IPArray[3],
                                                                                   t_configInfo.PortServerConnecting); // TODOCM remove test code
            }
            else
            {
                m_serverConnection.ConnectingAdress = t_networkModule.CreateAdress(127, 0, 0, 1, t_configInfo.PortServerConnecting);
            }

            // Create new socket for unreliable
            m_serverConnection.ConnectingSocketHandle = t_networkModule.CreateUnreliableSocket();

            delete[] t_IPCharPointer;
            delete[] t_IPArray;
        }

        NetworkConnectionsClient::~NetworkConnectionsClient() {}
    }
}