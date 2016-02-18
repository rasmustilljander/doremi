#pragma once
#include <cstdint>
#include <memory>
#include <Utility/Utilities/Include/Memory/Circlebuffer/ArbitraryStaticData.hpp>
#include <Utility/Utilities/Include/PointerArithmetic/PointerArithmetic.hpp>
#include <mutex>
#include <Utility/Utilities/Include/IO/Mutex/Mutex.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/CircleBufferHeader.hpp>
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            class ArbitrarySizeCirclebuffer
            {
            public:
                /**
                TODO docs
                */
                ArbitrarySizeCirclebuffer()
                    : m_data(nullptr),
                      m_rawBufferPointerStart(nullptr),
                      m_rawBufferPointerEnd(nullptr),
                      m_adjustedBufferPointerStart(nullptr),
                      m_adjustedBufferPointerEnd(nullptr),
                      m_rawBufferSize(0),
                      m_adjustedBufferSize(0),
                      m_internalMemoryManagement(true),
                      m_alreadyInitialized(false),
                      m_mutex(nullptr)
                {
                }

                /**
                TODO docs
                */
                virtual ~ArbitrarySizeCirclebuffer()
                {
                    if(m_internalMemoryManagement)
                    {
                        free(m_rawBufferPointerStart);
                    }
                }

                /**
                The useable size of the buffer will be p_bufferSize - sizeof(StaticData).
                Each object will allocate sizeof(T) + sizeof(CircleBufferHeader) each
                */
                void Initialize(const size_t& p_bufferSize, IO::Mutex* p_mutex = nullptr)
                {
                    AssertInitialize(p_bufferSize);
                    m_rawBufferSize = p_bufferSize;
                    m_mutex = p_mutex;
                    m_rawBufferPointerStart = malloc(m_rawBufferSize);
                    SetupVariables();
                }
                /**
                The useable size of the buffer will be p_bufferSize - sizeof(StaticData)
                Each object will allocate sizeof(T) + sizeof(CircleBufferHeader) each
                */
                void Initialize(void* const p_preAllocatedBuffer, const size_t& p_bufferSize, IO::Mutex* p_mutex = nullptr)
                {
                    AssertInitialize(p_bufferSize);
                    m_rawBufferSize = p_bufferSize;
                    m_rawBufferPointerStart = p_preAllocatedBuffer;
                    m_mutex = p_mutex;
                    m_internalMemoryManagement = false;
                    SetupVariables();
                }

                /**
                Threadsafe, internal loackage
                */
                template <typename T> bool Produce(const CircleBufferHeader& p_Header, const T* const p_data) // TODORT, does this not trigger
                {
                    // Internal lockage
                    std::lock_guard<std::mutex> lock(m_produceLock);

                    const size_t desiredSizeToProduce = sizeof(p_Header) + p_Header.packageSize;

                    // Filemap lock
                    LockMetaData();

                    // Check if there's space
                    if(m_data->allocatedMemory + desiredSizeToProduce <= m_data->totalMemory)
                    {
                        // Compute current head
                        void* head = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentHeadOffset);

                        // Check if current head is out of our scope
                        if(PointerArithmetic::AssertAdresstInside(head, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                        {
                            // If current head is out of our scope AND we already know that there is space in the buffer, then it is ok to reset head
                            // to start.
                            head = m_adjustedBufferPointerStart;
                        }

                        // Compute the desiredHead (aka, the head that will be after this copy)
                        void* desiredHead = PointerArithmetic::Addition(head, desiredSizeToProduce);

                        // Check if desired head is out of scope
                        if(PointerArithmetic::AssertAdresstInside(desiredHead, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                        {
                            // Specialcase! If this happens
                            // Check if possible to place head in the end
                            // .if possible
                            // ..check if possible to place package in the start
                            // ...if possible
                            // ....write header at current head
                            // ....write package in the front
                        }

                        //// Produce is ok as usual
                        // Copy metaheader to current_head
                        memcpy(head, &p_Header, sizeof(CircleBufferHeader));

                        // Move head to after metaheader
                        head = PointerArithmetic::Addition(head, sizeof(CircleBufferHeader));

                        // Copy data to head
                        memcpy(head, p_data, p_Header.packageSize);

                        // Move head to after data
                        head = PointerArithmetic::Addition(head, p_Header.packageSize);

                        // Update global head
                        m_data->currentHeadOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, head);

                        // Update allocation information
                        m_data->allocatedMemory += desiredSizeToProduce;

                        // Unlock filemap
                        UnLockMetaData();

                        // Complete
                        return true;
                    }
                    else
                    {
                        // Unlock filemap
                        UnLockMetaData();

                        // Failed
                        return false;
                    }
                }

                /**
                Not threadsafe
                */
                bool Consume(CircleBufferHeader*& out_header, void*& out_data)
                {
                    // Internal lockage
                    std::lock_guard<std::mutex> lock(m_consumeLock);

                    // Filemap lock
                    LockMetaData();
                    if(m_data->allocatedMemory > 0)
                    {
                        // Compute current tail
                        void* tail = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentTailOffset);
                        void* desiredTailAfterHeader = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                        // Check if current tail is outside our scope.
                        if(PointerArithmetic::AssertAdresstInside(tail, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                        { // Not sure if this function is required
                            //// Cover the case where all the data can be found at the beginning

                            // If this happens
                            // .read header at the beginning of the buffer
                            // .check if package can fit after header
                            // ..true if (( totalMemory - (end - tail) - sizeof(CircleBufferHeader)) > sizeofPackage)
                            // ...read package after header in beginning of buffer
                            // ..else false
                            //...not enough space :C
                        }

                        // Check if desired tail is outside our scope
                        else if(PointerArithmetic::AssertAdresstInside(desiredTailAfterHeader, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                        {
                            //// Cover the case
                            //// 1. where the header can be found before end and the package at the beginning

                            // Specialcase! If this happens
                            // Read header at current header
                            // .check if package can fit from desiredTailAfterHeader to end
                            // ..if possible
                            // ...read package as usual
                            // ..if not possible
                            // ...read package at the beginning of the buffer
                        }

                        //// Produce like no tomorrow, aka, everything can be found at the sameplace, the tail.

                        // Copy metaheader to current_head
                        memcpy(out_header, tail, sizeof(CircleBufferHeader));

                        // Move header to after metaheader
                        tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                        // Copy data to current:_head
                        memcpy(out_data, tail, out_header->packageSize);

                        // Move header to after data
                        tail = PointerArithmetic::Addition(tail, out_header->packageSize);

                        // Update global tail
                        m_data->currentTailOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, tail);

                        // Update allocation information
                        // TODOXX Checkoverflow
                        // TODORT
                        m_data->allocatedMemory -= (sizeof(CircleBufferHeader) + out_header->packageSize);

                        // Unlock filemap
                        UnLockMetaData();

                        return true;
                    }
                    else
                    {
                        // Unlock filemap
                        UnLockMetaData();

                        // Nothing to do
                        return false;
                    }
                }

            protected:
                void AssertInitialize(const size_t& p_bufferSize)
                {
                    if(m_alreadyInitialized)
                    {
                        throw std::runtime_error("Already initialized.");
                    }

                    const uint32_t minimumSize = sizeof(CircleBufferHeader) + sizeof(ArbitraryStaticData);
                    if(minimumSize < p_bufferSize)
                    {
                        // TODORT better messages
                        const std::string errorMessage = std::string("Not enough space.");
                        throw std::runtime_error(errorMessage);
                    }
                }

                void LockMetaData()
                {
                    if(m_mutex != nullptr)
                    {
                        m_mutex->lock();
                    }
                }

                void UnLockMetaData()
                {
                    if(m_mutex != nullptr)
                    {
                        m_mutex->unlock();
                    }
                }

                void ComputeAdjustments()
                {
                    m_adjustedBufferSize = m_rawBufferSize - sizeof(ArbitraryStaticData);

                    m_adjustedBufferPointerStart = PointerArithmetic::Addition(m_rawBufferPointerStart, sizeof(ArbitraryStaticData));
                    m_adjustedBufferPointerEnd = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_adjustedBufferSize);
                }

                void SetupVariables()
                {
                    m_alreadyInitialized = true;
                    m_rawBufferPointerEnd = PointerArithmetic::Addition(m_rawBufferPointerStart, m_rawBufferSize);
                    ComputeAdjustments();
                    m_data = static_cast<ArbitraryStaticData*>(m_rawBufferPointerStart);

                    // TODORT
                    // LockMetaData In a very specific state both processes may come here at about the same time.
                    // However, that should not be a problem. If they're almost here at the same no data should be lost.
                    if(m_data->started != ARBITRARY_CIRCLE_BUFFER_STARTED)
                    {
                        *m_data = ArbitraryStaticData();
                        m_data->currentTailOffset = 0;
                        m_data->currentHeadOffset = 0;
                    }
                    // TODORT
                    // UnLockMetaData
                }

                ArbitraryStaticData* m_data;
                void* m_rawBufferPointerStart;
                void* m_rawBufferPointerEnd;
                void* m_adjustedBufferPointerStart;
                void* m_adjustedBufferPointerEnd;
                size_t m_rawBufferSize;
                size_t m_adjustedBufferSize;
                bool m_internalMemoryManagement;
                std::mutex m_produceLock;
                std::mutex m_consumeLock;
                bool m_alreadyInitialized;
                IO::Mutex* m_mutex;
            };
        }
    }
}