#pragma once
#include <cstdint>
#include <Utility/Utilities/Include/PointerArithmetic/PointerArithmetic.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/CircleBufferHeader.hpp>
#include <mutex>

namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            struct ArbitraryStaticData;

            class ArbitrarySizeCirclebuffer
            {
            public:
                /**
                TODO docs
                */
                ArbitrarySizeCirclebuffer();

                /**
                TODO docs
                */
                virtual ~ArbitrarySizeCirclebuffer();

                /**
                The useable size of the buffer will be p_bufferSize - sizeof(StaticData).
                Each object will allocate sizeof(T) + sizeof(CircleBufferHeader).
                */
                void Initialize(const uint32_t& p_bufferSize, std::mutex* p_metaDataMutex = nullptr);

                /**
                The useable size of the buffer will be p_bufferSize - sizeof(StaticData).
                Each object will allocate sizeof(T) + sizeof(CircleBufferHeader).
                */
                void Initialize(void* const p_preAllocatedBuffer, const uint32_t& p_bufferSize, std::mutex* p_metaDataMutex = nullptr);

                /**
                Threadsafe, internal loackage. Throws exception if not possible.
                */
                void Produce(const CircleBufferHeader& p_Header, const void* const p_data);

                /**
                Threadsafe, internal loackage. Returns false if there was nothing to read.
                */
                bool Consume(CircleBufferHeader*& o_header, void* o_dataBuffer, const uint32_t& p_outbufferSize);

            protected:
                void AssertInitialize(const uint32_t& p_bufferSize);

                void LockMetaData();

                void UnLockMetaData();

                void ComputeAdjustments();

                void SetupVariables();
                void ResetMetaData();

                uint32_t ComputeAvilableSpace(const uint32_t& p_requestedSize);

                void ComputeBufferLocationForProduce(const uint32_t& p_requestedSize, const uint32_t& currentlyAvailableSpace, bool& o_headerAtEnd, bool& o_dataAtEnd);

                uint32_t ComputeDiffBetweenProducedAndConsumed();

                // TODORT can be moved to utility
                static bool CheckIfBothIsTrueOrBothIsFalse(bool a, bool b) { return (a == b); }

                ArbitraryStaticData* m_data;
                void* m_rawBufferPointerStart;
                void* m_rawBufferPointerEnd;
                void* m_adjustedBufferPointerStart;
                void* m_adjustedBufferPointerEnd;
                uint32_t m_rawBufferSize;
                uint32_t m_adjustedBufferSize;
                bool m_internalMemoryManagement;
                std::mutex m_produceLock;
                std::mutex m_consumeLock;
                bool m_alreadyInitialized;
                std::mutex* m_metaDataMutex;
            };
        }
    }
}