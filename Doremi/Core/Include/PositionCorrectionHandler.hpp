#pragma once
#include <DirectXMath.h>
#include <list>
#include <vector>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        /**
            TODOCM doc
        */
        struct PositionStamp
        {
            PositionStamp(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_movement, uint8_t p_sequence)
                : Position(p_position), Orientation(p_orientation), Movement(p_movement), Sequence(p_sequence)
            {

            }

            uint8_t Sequence;
            DirectX::XMFLOAT3 Position;
            DirectX::XMFLOAT4 Orientation;
            DirectX::XMFLOAT3 Movement;

            bool operator==(const uint8_t &p_sequence) const
            {
                return Sequence == p_sequence;
            }
        };

        /**
            TODOCM doc
        */
        class PositionCorrectionHandler
        {
        public:
            /**
                TODOCM doc
            */
            static PositionCorrectionHandler* GetInstance();

            /**
                TODOCM doc
            */
            static void StartPositionCorrectionHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            void CheckPositionFromServer(uint32_t p_playerID, DirectX::XMFLOAT3 p_positionToCheck, uint8_t p_sequenceOfPosition);

            /**
                TODOCM doc
            */
            void QueuePlayerPositionForCheck(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_movement, uint8_t p_sequence);

        private:
            /**
                TODOCM doc
            */
            PositionCorrectionHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            ~PositionCorrectionHandler();

            /**
                TODOCM doc
            */
            static PositionCorrectionHandler* m_singleton;

            /**
                TODOCM doc
            */
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            /**
                TODOCM doc
            */
            std::list<PositionStamp> m_PositionStamps;
        };
    }
}