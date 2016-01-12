#pragma once
#include <list>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>

#define MAX_OBJECTS_IN_SNAPSHOT 100

namespace Doremi
{
    namespace Core
    {
        struct SnapshotObject
        {
            uint32_t EntityID;
            TransformComponentNext Component;
        };

        struct Snapshot
        {
            uint8_t SnapshotSequence;
            uint8_t NumOfObjects;
            SnapshotObject Objects[MAX_OBJECTS_IN_SNAPSHOT];
        };

        struct PositionStamp
        {
            PositionStamp(uint8_t p_sequence, DirectX::XMFLOAT3 p_position) : Sequence(p_sequence), Position(p_position) {}
            uint8_t Sequence;
            DirectX::XMFLOAT3 Position;
        };

        class InterpolationHandler
        {
            public:
            /**
                TODOCM doc
            */
            static InterpolationHandler* GetInstance();

            /**
                TODOCM doc
            */
            void InterpolateFrame(double p_alpha);

            /**
                TODOCM doc
            */
            void QueueSnapshot(Snapshot* p_newSnapshot);

            /**
                TODOCM doc
            */
            void UpdateInterpolationTransforms();

            /**
                TODOCM doc
            */
            void SetSequence(uint8_t p_sequence);

            /**
                TODOCM doc
            */
            uint8_t GetRealSnapshotSequence();

            /**
                TODOCM doc
            */
            void QueuePlayerPositionForCheck(DirectX::XMFLOAT3 p_position);

            /**
                TODOCM doc
            */
            void CheckPositionFromServer(DirectX::XMFLOAT3 p_positionToCheck, uint8_t p_sequenceOfPosition);

            private:
            /**
                TODOCM doc
            */
            InterpolationHandler();

            /**
                TODOCM doc
            */
            ~InterpolationHandler();

            /**
                TODOCM doc
            */
            static InterpolationHandler* m_singleton;

            /**
                TODOCM doc
            */
            uint8_t m_snapshotSequenceReal;

            /**
                TODOCM doc
            */
            uint8_t m_snapshotSequenceUsed;

            /**
                TODOCM doc
            */
            uint8_t m_snapshotDelay;
            /**
                TODOCM doc
            */
            std::list<Snapshot*> m_DelayedSnapshots;

            /**
                TODOCM doc
            */
            uint8_t m_NumOfSequencesToInterpolate;

            /**
                TODOCM doc
            */
            uint8_t m_SequenceInterpolationOffset;

            /**
                TODOCM doc
            */
            std::list<PositionStamp> m_PositionStamps;
        };
    }
}