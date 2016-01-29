#pragma once
#include <list>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DirectXMath.h>
#include <iostream>

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

        static bool SnapObjIDMoreRecent(const SnapshotObject& p_1, const SnapshotObject& p_2) { return p_1.EntityID < p_2.EntityID; }

        /**
            TODOCM doc
        */
        static TransformComponentNext ExtrapolateTransform(TransformComponentSnapshotPrevious* p_prev, TransformComponentSnapshotNext* p_next, float alpha)
        {
            TransformComponentNext o_next;

            float realAlpha = 0;

            if(p_prev->framesToNext != 0)
            {
                realAlpha = ((static_cast<float>(p_next->framesToNextCounter) + alpha - 1) / static_cast<float>(p_prev->framesToNext)) + 1;
            }
            if(p_next->framesToNextCounter == 0)
            {
                std::cout << "second 0" << std::endl;
            }

            // Extrapolate with interpolate function where T is larger then one
            DirectX::XMStoreFloat3(&o_next.position,
                                   DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&p_prev->position), DirectX::XMLoadFloat3(&p_next->position), realAlpha));


            // Extrapolate orientation
            // TODOCM not sure if this works or not, if you can extrapolate with SLER
            DirectX::XMStoreFloat4(&o_next.rotation, DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&p_prev->rotation),
                                                                                DirectX::XMLoadFloat4(&p_next->rotation), realAlpha));

            // Extrapolate scale
            DirectX::XMStoreFloat3(&o_next.scale, DirectX::XMVectorLerp(XMLoadFloat3(&p_prev->scale), DirectX::XMLoadFloat3(&p_next->scale), realAlpha));

            return o_next;
        }

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
            static void StartInterpolationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

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

        private:
            /**
                TODOCM doc
            */
            InterpolationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

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
            const DoremiEngine::Core::SharedContext& m_sharedContext;

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

            
        };
    }
}