#include <PositionCorrectionHandler.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <iostream>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        PositionCorrectionHandler* PositionCorrectionHandler::m_singleton = nullptr;

        PositionCorrectionHandler* PositionCorrectionHandler::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                std::runtime_error("PositionCorrectionHandler called GetInstance befor startup.");
            }
            return m_singleton;
        }

        void PositionCorrectionHandler::StartPositionCorrectionHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if (m_singleton == nullptr)
            {
                m_singleton = new PositionCorrectionHandler(p_sharedContext);
            }
            else
            {
                std::runtime_error("Duplicate startup called on PositionCorrectionHandler.");
            }
        }

        PositionCorrectionHandler::PositionCorrectionHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {

        }

        PositionCorrectionHandler::~PositionCorrectionHandler()
        {

        }

        void PositionCorrectionHandler::CheckPositionFromServer(uint32_t p_playerID, DirectX::XMFLOAT3 p_positionToCheck, uint8_t p_sequenceOfPosition)
        {
            // Find the movement matching our sequence, and apply it and all the movements until our frame
            std::list<MovementStamp>::iterator iter;
            iter = std::find(m_PositionStamps.begin(), m_PositionStamps.end(), p_sequenceOfPosition);

            // Check if the one we entered is in buffer ( could be that we already recieved a previous.
            if(iter != m_PositionStamps.end())
            {
                EntityID playerEntityID = 0;
                if(!PlayerHandler::GetInstance()->GetEntityIDForPlayer(p_playerID, playerEntityID))
                {
                    std::cout << "Error player entityID in PositionCorrectionHandler" << std::endl;
                }

                // Set position to players
                DoremiEngine::Physics::CharacterControlManager& charControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();
                charControlManager.SetPosition(playerEntityID, p_positionToCheck);

                // Create reverse iterator
                std::list<MovementStamp>::reverse_iterator revIter = --std::list<MovementStamp>::reverse_iterator(iter);
                std::list<MovementStamp>::reverse_iterator revIterEnd = m_PositionStamps.rend();

                // For all inputs, we move character
                for(; revIter != revIterEnd; ++revIter)
                {
                    // TODOXX hard coded update timer 0.017 for position correction, if changed this will fuckup
                    charControlManager.MoveController(playerEntityID, revIter->Movement, 0.017f);
                }
            }

            // remove all the movements we've passed and the one we checked
            m_PositionStamps.erase(iter, m_PositionStamps.end());
        }

        void PositionCorrectionHandler::QueuePlayerPositionForCheck(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation,
                                                                    DirectX::XMFLOAT3 p_movement, uint8_t p_sequence)
        {
            m_PositionStamps.push_front(MovementStamp(p_position, p_orientation, p_movement, p_sequence));
        }
    }
}