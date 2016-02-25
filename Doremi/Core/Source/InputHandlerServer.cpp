#include <InputHandlerServer.hpp>
#include <SequenceMath.hpp>
#include <iostream>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

namespace Doremi
{
    namespace Core
    {
        // TODOXX sequencedelay must be more then snapshotdelay in interpolationhandler
        InputHandlerServer::InputHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT3 p_startingPosition)
            : InputHandler(p_sharedContext), m_sequenceDelay(10), m_positionByLastInput(p_startingPosition)
        {
        }

        void InputHandlerServer::QueueInput(uint32_t p_bitMask, DirectX::XMFLOAT4 p_orienation, uint8_t p_sequence)
        {
            // Check if our sequence hasn't passed
            if(sequence_more_recent(p_sequence, m_realSequence, 255))
            {
                // Try to fill it
                std::list<InputItem>::iterator iter = m_queuedInputs.begin();

                for(; iter != m_queuedInputs.end(); ++iter)
                {
                    // If we're more recent we put it here
                    if(sequence_more_recent(p_sequence, iter->Sequence, 255))
                    {
                        break;
                    }
                }

                // Insert it
                m_queuedInputs.insert(iter, InputItem(p_bitMask, p_orienation, p_sequence));
            }
            // TODOCM remove debugg
            if(m_queuedInputs.size() > 30)
            {
                cout << "Got more then 30 inputs buffered, real num: " << m_queuedInputs.size() << endl;
            }
        }

        void InputHandlerServer::Update(EntityID p_entityID)
        {
            // Increment realsequence
            m_realSequence++;

            // Holder for new input
            uint32_t newMaskWithInput = m_maskWithInput;

            // If we have any inputs
            std::list<InputItem>::reverse_iterator riter = m_queuedInputs.rbegin();

            // For all
            for(; riter != m_queuedInputs.rend(); ++riter)
            {
                // If we're more recent
                if(sequence_more_recent(riter->Sequence + 1, m_realSequence, 255))
                {
                    break;
                }
            }

            // Remove thospe we've skipped
            m_queuedInputs.erase(riter.base(), m_queuedInputs.end());
            riter = m_queuedInputs.rbegin();

            if(m_queuedInputs.size())
            {
                // Check if realsequence is same as last queued
                if(riter->Sequence == m_realSequence)
                {
                    // Set inputmask
                    newMaskWithInput = riter->InputBitmask;

                    // Set quaternion
                    m_orientationQuaternion = riter->OrientationQuaternion;

                    // Remove sequence
                    m_queuedInputs.pop_back();

                    // Now we want to save position and sequence to server to update
                    m_positionByLastInput = GetComponent<TransformComponent>(p_entityID)->position;
                    // m_positionByLastInput = m_sharedContext.GetPhysicsModule().GetCharacterControlManager().GetPosition(p_entityID);

                    m_sequenceByLastInput = m_realSequence;
                }
            }

            // Update new and last sequence
            m_lastUpdateMaskWithInput = m_maskWithInput;
            m_maskWithInput = newMaskWithInput;
        }

        void InputHandlerServer::SetSequence(uint8_t p_sequence) { m_realSequence = p_sequence - m_sequenceDelay; }

        void InputHandlerServer::ClearInput()
        {
            m_queuedInputs.clear();
            m_maskWithInput = 0;
        }

        DirectX::XMFLOAT4 InputHandlerServer::GetOrientationFromInput() { return m_orientationQuaternion; }
    }
}