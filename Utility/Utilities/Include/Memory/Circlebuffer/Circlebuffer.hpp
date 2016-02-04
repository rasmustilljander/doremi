#pragma once
#include <cstdint>
#include <memory>
#include <Utility/Utilities/Include/Memory/Circlebuffer/StaticData.hpp>
#include <Utility/Utilities/Include/PointerArithmetic/PointerArithmetic.hpp>
#include <mutex>
#include <Utility/Utilities/Include/IO/Mutex/Mutex.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/CircleBufferHeader.hpp>

// Move to Logging lib
struct LogText
{
    int8_t consoleId;
    //	enum logFlag;
    //	enum logLevel;
    int8_t messageLength;
    char message[256];
};

// Move to Logging lib
struct CustomCommand
{
    char message[256];
};

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
                The useable size of the buffer will be p_bufferSize - sizeof(size_t)
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
                The useable size of the buffer will be p_bufferSize - sizeof(size_t)
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
                bool Produce(const CircleBufferHeader& p_Header, const T const p_data)
                {
                    // Internal lockage
                    std::lock_guard<std::mutex> lock(m_produceLock);

                    // Find if it is possible to produce more
                    // Does not require to lock metadata under the assumption that when circlebuffers communicate via IPC one side is only producer
                    // and the other side is only comsumer.
                    // And as this part is already locked with internal lockage above
                    if(m_data->currentObjectCount < m_maxContainedObjects)
                    {
                        // Local head
                        void* head = m_data->currentHead;

                        // Check if current pointer is outside our scope.
                        if(PointerArithmetic::AssertAdresstInside(head, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                        {
                            head = m_adjustedBufferPointerStart; // If so, reset to first part.
                        }

                        //// Produce is ok
                        // Copy metaheader to current_head
                        memcpy(head, &p_Header, sizeof(CircleBufferHeader));

                        // Move head to after metaheader
                        head = PointerArithmetic::Addition(head, sizeof(CircleBufferHeader));

                        // Copy data to head
                        memcpy(head, &p_data, p_Header.packageSize);

                        // Move head to after data
                        head = PointerArithmetic::Addition(head, p_Header.packageSize);

                        // Update global head
                        m_data->currentHead = head;

                        // Filemap lock
                        LockMetaData();

                        // Update object count
                        m_data->currentObjectCount++;

                        // Unlock filemap
                        UnLockMetaData();

                        // Complete
                        return true;
                    }
                    else
                    {
                        // Failed
                        return false;
                    }
                }

                /**
                Not threadsafe
                */
                bool Consume(CircleBufferHeader*& out_header, T*& out_data)
                {
                    // Internal lockage
                    std::lock_guard<std::mutex> lock(m_consumeLock);

                    // Find if it is possible to consume anything
                    // Does not require to lock metadata under the assumption that when circlebuffers communicate via IPC one side is only producer
                    // and the other side is only comsumer.
                    // And as this part is already locked with internal lockage above
                    if(m_data->currentObjectCount > 0)
                    {
                        // Local tail
                        void* tail = m_data->currentTail;

                        // Check if current pointer is outside our scope.
                        if(PointerArithmetic::AssertAdresstInside(tail, m_adjustedBufferPointerStart, m_adjustedBufferPointerEnd) == false)
                        {
                            tail = m_adjustedBufferPointerStart; // If so, reset to first part.
                        }

                        //// Produce is ok
                        // Copy metaheader to current_head
                        memcpy(out_header, tail, sizeof(CircleBufferHeader));

                        // Move header to after metaheader
                        tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                        // Copy data to current:_head
                        memcpy(out_data, tail, out_header->packageSize);

                        // Move header to after data
                        tail = PointerArithmetic::Addition(tail, out_header->packageSize);

                        // Update global tail
                        m_data->currentTail = tail;

                        // Filemap lock
                        LockMetaData();

                        // Update object count
                        m_data->currentObjectCount--;

                        // Unlock filemap
                        UnLockMetaData();

                        return true;
                    }
                    else
                    {
                        // Nothing to do
                        return false;
                    }
                }

            protected:
                void CircleBuffer::AssertInitialize(const size_t& p_bufferSize)
                {
                    m_sizeOfOneObject = sizeof(T) + sizeof(CircleBufferHeader);
                    if(m_sizeOfOneObject > (p_bufferSize - sizeof(StaticData)))
                    {
                        throw std::runtime_error("Not enough space.");
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
                    m_rawBufferPointerEnd = PointerArithmetic::Addition(m_rawBufferPointerStart, m_rawBufferSize);
                    ComputeAdjustments();
                    ComputeMaxObjects();
                    m_data = static_cast<StaticData*>(m_rawBufferPointerStart);
                    *m_data = StaticData();

                    // TODORT This is currently done on both sides, right / wrong?
                    if(m_data->currentTail == nullptr)
                    {
                        m_data->currentTail = m_adjustedBufferPointerStart;
                    }
                    if(m_data->currentHead == nullptr)
                    {
                        m_data->currentHead = m_adjustedBufferPointerStart;
                    }
                    m_alreadyInitialized = true;
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