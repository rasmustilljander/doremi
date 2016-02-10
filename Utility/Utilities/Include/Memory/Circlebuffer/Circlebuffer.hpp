#pragma once
#include <cstdint>
#include <memory>
#include <Utility/Utilities/Include/Memory/Circlebuffer/StaticData.hpp>
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
            template <typename T> class CircleBuffer
            {
            public:
                /**
                TODO docs
                */
                CircleBuffer()
                    : m_data(nullptr),
                      m_rawBufferPointerStart(nullptr),
                      m_rawBufferPointerEnd(nullptr),
                      m_adjustedBufferPointerStart(nullptr),
                      m_adjustedBufferPointerEnd(nullptr),
                      m_rawBufferSize(0),
                      m_adjustedBufferSize(0),
                      m_internalMemoryManagement(true),
                      m_alreadyInitialized(false),
                      m_maxContainedObjects(0),
                      m_mutex(nullptr),
                      m_sizeOfOneObject(0)
                {
                }

                /**
                TODO docs
                */
                virtual ~CircleBuffer()
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
                bool Produce(const CircleBufferHeader& p_Header, const T* const p_data) // TODORT, does this not trigger
                {
                    //// Internal lockage
                    // std::lock_guard<std::mutex> lock(m_produceLock);

                    //// Find if it is possible to produce more
                    //// Does not require to lock metadata under the assumption that when circlebuffers communicate via IPC one side is only producer
                    //// and the other side is only comsumer.
                    //// And as this part is already locked with internal lockage above
                    // if(m_data->currentObjectCount < m_maxContainedObjects)
                    //{
                    //    // Local head
                    //    void* head = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentHeadOffset);

                    //    // Check if current pointer is outside our scope.
                    //    if(PointerArithmetic::AssertAdresstInside(head, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                    //    {
                    //        head = m_adjustedBufferPointerStart; // If so, reset to first part.
                    //    }

                    //    //// Produce is ok
                    //    // Copy metaheader to current_head
                    //    memcpy(head, &p_Header, sizeof(CircleBufferHeader));

                    //    // Move head to after metaheader
                    //    head = PointerArithmetic::Addition(head, sizeof(CircleBufferHeader));

                    //    // Copy data to head
                    //    memcpy(head, p_data, p_Header.packageSize);

                    //    // Move head to after data
                    //    head = PointerArithmetic::Addition(head, p_Header.packageSize);

                    //    // Update global head
                    //    m_data->currentHeadOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, head);

                    //    // Filemap lock
                    //    LockMetaData();

                    //    // Update object count
                    //    m_data->currentObjectCount++;

                    //    // Unlock filemap
                    //    UnLockMetaData();

                    //    // Complete
                    //    return true;
                    //}
                    // else
                    //{
                    //    // Failed
                    //    return false;
                    //}
                    return true;
                }

                /**
                Not threadsafe
                */
                bool Consume(CircleBufferHeader*& out_header, T*& out_data)
                {
                    //// Internal lockage
                    // std::lock_guard<std::mutex> lock(m_consumeLock);

                    //// Find if it is possible to consume anything
                    //// Does not require to lock metadata under the assumption that when circlebuffers communicate via IPC one side is only producer
                    //// and the other side is only comsumer.
                    //// And as this part is already locked with internal lockage above
                    // if(m_data->currentObjectCount > 0)
                    //{
                    //    // Local tail
                    //    void* tail = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentTailOffset);

                    //    // Check if current pointer is outside our scope.
                    //    if(PointerArithmetic::AssertAdresstInside(tail, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                    //    {
                    //        tail = m_adjustedBufferPointerStart; // If so, reset to first part.
                    //    }

                    //    //// Produce is ok
                    //    // Copy metaheader to current_head
                    //    memcpy(out_header, tail, sizeof(CircleBufferHeader));

                    //    // Move header to after metaheader
                    //    tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                    //    // Copy data to current:_head
                    //    memcpy(out_data, tail, out_header->packageSize);

                    //    // Move header to after data
                    //    tail = PointerArithmetic::Addition(tail, out_header->packageSize);

                    //    // Update global tail
                    //    m_data->currentTailOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, tail);

                    //    // Filemap lock
                    //    LockMetaData();

                    //    // Update object count
                    //    m_data->currentObjectCount--;

                    //    // Unlock filemap
                    //    UnLockMetaData();

                    //    return true;
                    //}
                    // else
                    //{
                    //    // Nothing to do
                    //    return false;
                    //}
                    return true;
                }

            protected:
                void CircleBuffer::AssertInitialize(const size_t& p_bufferSize)
                {
                    m_sizeOfOneObject = sizeof(T) + sizeof(CircleBufferHeader);
                    if(m_sizeOfOneObject > (p_bufferSize - sizeof(StaticData)))
                    {
                        const std::string errorMessage =
                            std::string("Not enough space. Each object will take up sizeof(T) + sizeof(CircleBufferHeader) which is " +
                                        std::to_string(sizeof(T)) + "+" + std::to_string(sizeof(CircleBufferHeader)) + "=" +
                                        std::to_string(m_sizeOfOneObject) + ". This buffer only has access to " + std::to_string(p_bufferSize) + "-" +
                                        std::to_string(sizeof(StaticData)) + "=" + std::to_string(p_bufferSize - sizeof(StaticData)));
                        throw std::runtime_error(errorMessage);
                    }
                    if(m_alreadyInitialized)
                    {
                        throw std::runtime_error("Already initialized.");
                    }
                }

                void CircleBuffer::LockMetaData()
                {
                    if(m_mutex != nullptr)
                    {
                        m_mutex->AttemptLock();
                    }
                }

                void CircleBuffer::UnLockMetaData()
                {
                    if(m_mutex != nullptr)
                    {
                        m_mutex->Unlock();
                    }
                }

                void CircleBuffer::ComputeAdjustments()
                {
                    m_adjustedBufferSize = m_rawBufferSize - sizeof(StaticData);
                    m_adjustedBufferSize = (m_adjustedBufferSize / m_sizeOfOneObject) * m_sizeOfOneObject;

                    m_adjustedBufferPointerStart = PointerArithmetic::Addition(m_rawBufferPointerStart, sizeof(StaticData));
                    m_adjustedBufferPointerEnd = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_adjustedBufferSize);
                }

                void CircleBuffer::ComputeMaxObjects() { m_maxContainedObjects = m_adjustedBufferSize / m_sizeOfOneObject; }

                void CircleBuffer::SetupVariables()
                {
                    m_alreadyInitialized = true;
                    m_rawBufferPointerEnd = PointerArithmetic::Addition(m_rawBufferPointerStart, m_rawBufferSize);
                    ComputeAdjustments();
                    ComputeMaxObjects();
                    m_data = static_cast<StaticData*>(m_rawBufferPointerStart);

                    // TODORT
                    // LockMetaData In a very specific state both processes may come here at about the same time.
                    // However, that should not be a problem. If they're almost here at the same no data should be lost.
                    if(m_data->started != STARTED)
                    {
                        *m_data = StaticData();
                        m_data->currentTailOffset = 0;
                        m_data->currentHeadOffset = 0;
                    }
                    // TODORT
                    // UnLockMetaData
                }

                StaticData* m_data;
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
                size_t m_maxContainedObjects;
                IO::Mutex* m_mutex;
                size_t m_sizeOfOneObject;
            };
        }
    }
}