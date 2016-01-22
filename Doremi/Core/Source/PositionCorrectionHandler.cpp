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
            /**
            Notes:
                We need to interpolate the players position to a new position => hence why it should be in Interpolation handler.
                We need to store the playerID to set the position of the player
            */

            

            //// TODOCM remove this hotfix, and use list of inputs to check against, and set a interpolation point
            //// Check if we find the playerID

            //// If we have character controller
            //if (Core::EntityHandler::GetInstance().HasComponents(playerID, (int)ComponentType::CharacterController))
            //{
            //    // Set position
            //    m_sharedContext.GetPhysicsModule().GetCharacterControlManager().SetPosition(playerID, p_positionToCheck);
            //    // cout << "set position" << endl;
            //}


            // We get a sequence we probobly have saved?
            // We should find it, and apply everyone after for the set
            std::list<PositionStamp>::iterator iter;
            iter = std::find(m_PositionStamps.begin(), m_PositionStamps.end(), p_sequenceOfPosition);

            //std::cout << m_PositionStamps.size() << std::endl;

            //cout << "Check vs: " << (uint32_t)p_sequenceOfPosition << endl;

            // Check if the one we entered is in buffer ( could be that we already recieved a previous.
            if (iter != m_PositionStamps.end())
            {
                EntityID playerEntityID = 0;
                if (!PlayerHandler::GetInstance()->GetEntityIDForPlayer(p_playerID, playerEntityID))
                {
                    std::cout << "Error player entityID in PositionCorrectionHandler" << std::endl;
                }

                // Set position to players
                DoremiEngine::Physics::CharacterControlManager &charControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();

                XMFLOAT3 pos = charControlManager.GetPosition(playerEntityID);

                charControlManager.SetPosition(playerEntityID, p_positionToCheck);

                //cout << p_positionToCheck.x - iter->Position.x << " " << p_positionToCheck.y - iter->Position.y << " " << p_positionToCheck.z - iter->Position.z << endl;

                XMFLOAT3 pos1 = charControlManager.GetPosition(playerEntityID);



                // Create reverse iterator
                std::list<PositionStamp>::reverse_iterator revIter = --std::list<PositionStamp>::reverse_iterator(iter);
                std::list<PositionStamp>::reverse_iterator revIterEnd = m_PositionStamps.rend();

                int laps = 0;
                XMFLOAT3 pos2 = pos1;
                // For all inputs, we move character
                for (; revIter != revIterEnd; ++revIter)
                {
                    // TODOXX hard coded update timer 0.017 for position correction, if changed this will fuckup
                    charControlManager.MoveController(playerEntityID, revIter->Movement, 0.017f);

                    pos2 = charControlManager.GetPosition(playerEntityID);
                    laps++;
                    
                }

                /*if (abs(pos.x - pos2.x) > 0.01f || abs(pos.y - pos.y) > 0.01f || abs(pos.z - pos2.z) > 0.01f)
                {
                    
                    cout << "Previous:  " << pos.x << " " << pos.y << " " << pos.z << endl;
                    cout << "ServerPos: " << pos1.x << " " << pos1.y << " " << pos1.z << endl;
                    cout << "EndPos:    " << pos2.x << " " << pos2.y << " " << pos2.z << endl;
                    cout << "Inputs: " << laps << endl;
                }*/
            }


            m_PositionStamps.erase(iter, m_PositionStamps.end());
        }

        void PositionCorrectionHandler::QueuePlayerPositionForCheck(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_movement, uint8_t p_sequence)
        {
            m_PositionStamps.push_front(PositionStamp(p_position, p_orientation, p_movement, p_sequence));
        }
    }
}