#include <Doremi/Core/Include/EntityComponent/EntityHandlerServer.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>


namespace Doremi
{
    namespace Core
    {
        EntityHandlerServer::EntityHandlerServer() {}

        EntityHandlerServer::~EntityHandlerServer() {}

        void EntityHandlerServer::StartupEntityHandlerServer()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new EntityHandlerServer();
            }
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID);

            PlayerHandler::GetInstance()->QueueAddObjectToPlayers((uint32_t)p_blueprintID, DirectX::XMFLOAT3(0, 0, 0));

            return outID;
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID, p_position);

            PlayerHandler::GetInstance()->QueueAddObjectToPlayers((uint32_t)p_blueprintID, p_position);

            return outID;
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID, p_position, p_orientation);

            PlayerHandler::GetInstance()->QueueAddObjectToPlayers((uint32_t)p_blueprintID, p_position);

            return outID;
        }

        void EntityHandlerServer::RemoveEntity(int p_entityID)
        {
            PlayerHandler::GetInstance()->QueueRemoveObjectToPlayers(p_entityID);

            EntityHandler::RemoveEntity(p_entityID);
        }
    }
}
