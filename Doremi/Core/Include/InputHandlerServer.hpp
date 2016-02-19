#pragma once
#include <InputHandler.hpp>
#include <DirectXMath.h>
#include <list>

namespace Doremi
{
    namespace Core
    {
        struct InputItem
        {
            InputItem(uint32_t p_inputBitmask, DirectX::XMFLOAT4 p_orientation, uint8_t p_sequence)
                : InputBitmask(p_inputBitmask), OrientationQuaternion(p_orientation), Sequence(p_sequence)
            {
            }
            uint32_t InputBitmask;
            DirectX::XMFLOAT4 OrientationQuaternion;
            uint8_t Sequence;
        };

        class InputHandlerServer : public InputHandler
        {
        public:
            /**
                TODOCM doc
            */
            InputHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT3 p_startingPosition);

            /**
                TODOCM doc
            */
            void QueueInput(uint32_t p_bitMask, DirectX::XMFLOAT4 p_orienatio, uint8_t p_sequence);

            /**
                TODOCM doc
            */
            void Update(EntityID p_entityID);

            /**
                TODOCM doc
            */
            void SetSequence(uint8_t p_sequence);

            /**
                TODOCM doc
            */
            void ClearInput();

            /**
                TODOCM doc
            */
            DirectX::XMFLOAT4 GetOrientationFromInput();

            /**
                TODOCM doc
            */
            uint8_t GetSequenceByLastInput() { return m_sequenceByLastInput; }

            /**
                TODOCM doc
            */
            DirectX::XMFLOAT3 GetPositionByLastInput() { return m_positionByLastInput; }

            uint8_t GetSequence() { return m_realSequence; }

        private:
            /**
                TODOCM doc
            */
            std::list<InputItem> m_queuedInputs;

            /**
                TODOCM doc
            */
            DirectX::XMFLOAT4 m_orientationQuaternion;

            /**
                TODOCM doc
            */
            uint8_t m_sequenceDelay;

            /**
                TODOCM doc
            */
            uint8_t m_realSequence;

            /**
                TODOCM doc
            */
            DirectX::XMFLOAT3 m_positionByLastInput;

            /**
                TODOCM doc
            */
            uint8_t m_sequenceByLastInput;
        };
    }
}