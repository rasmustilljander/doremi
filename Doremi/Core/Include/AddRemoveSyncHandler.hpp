#pragma once
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <DirectXMath.h>
#include <list>

namespace Doremi
{
    namespace Core
    {
        class BitStreamer;
        struct NetMessage;

        /**
            TODOCM doc
        */
        struct ObjectToAddOrRemove
        {
            bool AddObject;
            uint32_t BluePrintOrEntityID;
            DirectX::XMFLOAT3 Position;
        };

        /**
            TODOCM doc
        */
        class AddRemoveSyncHandler
        {
            public:
            /**
                TODOCM doc
            */
            AddRemoveSyncHandler();

            /**
                TODOCM doc
            */
            ~AddRemoveSyncHandler();

            /**
                TODOCM doc
            */
            void AddRemoveQueuedObjects();

            /**
                TODOCM doc
            */
            void QueueAddObject(uint32_t p_blueprint, DirectX::XMFLOAT3 p_position);

            /**
                TODOCM doc
            */
            void UpdateQueueWithSequence(uint8_t p_sequence);

            /**
                TODOCM doc
            */
            void QueueObjectsFromFrame();

            /**
                TODOCM doc
            */
            void QueueRemoveObject(uint32_t p_id);

            /**
                TODOCM doc
            */
            void CheckNewAddRemoves(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            /**
                TODOCM doc
            */
            void WriteAddRemoves(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten);

            /**
                TODOCM doc
            */
            uint8_t GetNextSequenceUsed();

            private:
            /**
                TODOCM doc
            */
            std::list<ObjectToAddOrRemove> m_frameQueuedObjects;

            /**
                TODOCM doc
            */
            uint8_t m_nextSequence;

            /**
                TODOCM doc
            */
            std::list<std::list<ObjectToAddOrRemove>> m_BufferedAddRemoveObjects;
        };
    }
}
