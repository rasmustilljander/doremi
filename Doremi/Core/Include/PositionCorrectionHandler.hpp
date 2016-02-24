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
        struct MovementStamp
        {
            MovementStamp(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_movement, uint8_t p_sequence)
                : Position(p_position), Orientation(p_orientation), Movement(p_movement), Sequence(p_sequence)
            {
            }

            uint8_t Sequence;
            DirectX::XMFLOAT3 Position;
            DirectX::XMFLOAT4 Orientation;
            DirectX::XMFLOAT3 Movement;

            bool operator==(const uint8_t& p_sequence) const { return Sequence == p_sequence; }
        };

        /**
            TODOCM doc
        */
        class PositionCorrectionHandler
        {
        public:
            /**
                StartPositionCorrectionHandler needs to be called befor this
            */
            static PositionCorrectionHandler* GetInstance();

            /**
                Needs to be called befor GetInstance
            */
            static void StartPositionCorrectionHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                Checks a position retrieved from server and sets the correction to the next interpolation goal
            */
            void InterpolatePositionFromServer(DirectX::XMFLOAT3 p_positionToCheck, uint8_t p_sequenceOfPosition);

            /**
                Doesn't extrapolate for now, but actually applies the physics position to the transform
            */
            void ExtrapolatePosition();

            /**
                Queues position and movement, used as we check vs server later
            */
            void QueuePlayerPositionForCheck(DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_movement, uint8_t p_sequence);

        private:
            /**
                Constructor
            */
            PositionCorrectionHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                Destructor
            */
            ~PositionCorrectionHandler();

            /**
                Singleton pointer
            */
            static PositionCorrectionHandler* m_singleton;

            /**
                Sharedcontext, used to get physics
            */
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            /**
                Buffered positions and movements from the past we can use for checking vs server pos
            */
            std::list<MovementStamp> m_PositionStamps;
        };
    }
}