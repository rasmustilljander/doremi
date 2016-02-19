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
                      m_metaDataMutex(nullptr)
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
                    if(m_metaDataMutex != nullptr)
                    {
                        delete m_metaDataMutex;
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
                    m_metaDataMutex = p_mutex;
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
                    m_metaDataMutex = p_mutex;
                    m_internalMemoryManagement = false;
                    SetupVariables();
                }

                size_t ComputeAvilableSpace(const size_t& p_requestedSize)
                {
                    const size_t diff = ComputeDiffBetweenProducedAndConsumed();
                    const size_t currentlyAvailableSpace = m_adjustedBufferSize - diff;
                    return currentlyAvailableSpace;
                }

                void ComputeBufferLocationForProduce(const size_t& p_requestedSize, const size_t& currentlyAvailableSpace, bool& o_headerAtEnd, bool& o_dataAtEnd)
                {
                    ///////////////
                    // Fetch the location of the current head
                    void* workingHead = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentHeadOffset);

                    // Compute the size of the memory between the head and the end.
                    const size_t sizeBetweenHeadAndEnd = PointerArithmetic::Difference(workingHead, m_adjustedBufferPointerEnd);

                    // Check if everything can be placed easily in the end.
                    if(p_requestedSize <= sizeBetweenHeadAndEnd)
                    {
                        //// Everything can fit at the end
                        o_headerAtEnd = true;
                        o_dataAtEnd = true;
                    }
                    else
                    {
                        //// Specialcase, find if possible to split package with metaheaderpackage at end and data in front or both in front.

                        // Compute the available data in the front
                        const size_t availableSpaceAtFront = currentlyAvailableSpace - sizeBetweenHeadAndEnd;

                        // Check if at least metaheader can be placed in the end.
                        if(sizeof(CircleBufferHeader) < sizeBetweenHeadAndEnd)
                        {
                            //// Header can be placed in the end
                            o_headerAtEnd = true;

                            // Check if data can be placed in front
                            const size_t dataSize = p_requestedSize - sizeof(CircleBufferHeader);
                            if(dataSize < availableSpaceAtFront)
                            {
                                //// Data can be placed in the front
                                o_dataAtEnd = false;
                            }
                            else
                            {
                                const std::string errorMessage = std::string("Not enough space in the buffer.");
                                throw std::runtime_error(errorMessage);
                            }
                        }
                        else
                        {
                            //// Header could not be placed in the end

                            // Check if everything can be placed in the front
                            if(p_requestedSize <= availableSpaceAtFront)
                            {
                                //// Everything fits in the front
                                o_headerAtEnd = false;
                                o_dataAtEnd = false;
                            }
                            else
                            {
                                const std::string errorMessage = std::string("Not enough space in the buffer.");
                                throw std::runtime_error(errorMessage);
                            }
                        }
                    }
                    ///////////////
                }

                /**
                Threadsafe, internal loackage. Throws exception if not possible.
                */
                void Produce(const CircleBufferHeader& p_Header, const void* const p_data)
                {
                    // Internal lockage
                    std::lock_guard<std::mutex> lock(m_produceLock);

                    const size_t requestedSize = sizeof(CircleBufferHeader) + p_Header.packageSize;
                    const size_t currentlyAvailableSpace = ComputeAvilableSpace(requestedSize);

                    if(currentlyAvailableSpace < requestedSize)
                    {
                        const std::string errorMessage = std::string("Not enough space in circlebuffer. Size of produce request " + std::to_string(requestedSize) +
                                                                     ". Only have access to " + std::to_string(currentlyAvailableSpace));
                        throw std::runtime_error(errorMessage);
                    }

                    bool headerAtEnd;
                    bool dataAtEnd;
                    ComputeBufferLocationForProduce(requestedSize, currentlyAvailableSpace, headerAtEnd, dataAtEnd);

                    // Reset workingHead
                    void* workingHead = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentHeadOffset);
                    void* locationOfMetaHeader;
                    void* locationOfData;

                    // Check if both is false or if both is true
                    if(CheckIfBothIsTrueOrBothIsFalse(headerAtEnd, dataAtEnd))
                    {
                        //// Both is true or both is false. Means that they are placed together, metadataheader first followed by the data.

                        // Check if they're placed in the end
                        if(headerAtEnd)
                        {
                            //// Both are placed in the end
                            locationOfMetaHeader = workingHead;
                        }
                        else
                        {
                            //// Both are placed infront
                            locationOfMetaHeader = m_adjustedBufferPointerStart;
                        }
                        // data is always right after
                        locationOfData = PointerArithmetic::Addition(locationOfMetaHeader, sizeof(CircleBufferHeader));
                    }
                    else
                    {
                        //// Means that the metadataheader is placed last and the data in front
                        locationOfMetaHeader = workingHead;
                        locationOfData = m_adjustedBufferPointerStart;
                    }

                    //// Helpdata complete, time to write
                    // Write header
                    memcpy(locationOfMetaHeader, &p_Header, sizeof(CircleBufferHeader));

                    // Write data
                    memcpy(locationOfData, p_data, p_Header.packageSize);

                    // Update shared head
                    workingHead = PointerArithmetic::Addition(locationOfData, p_Header.packageSize);

                    m_data->currentHeadOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, workingHead);
                    m_data->totalProducedMemoryInBytes += requestedSize;
                }

                /**
                Threadsafe, internal loackage. Returns false if there was nothing to read.
                */
                bool Consume(CircleBufferHeader*& o_header, void* o_dataBuffer, const uint32_t& p_outbufferSize)
                {
                    // Internal lockage
                    std::lock_guard<std::mutex> lock(m_consumeLock);
                    bool success = false;

                    const size_t diff = ComputeDiffBetweenProducedAndConsumed();
                    if(diff == 0)
                    {
                        return false;
                    }

                    // Fetch the location of the current tail
                    void* tail = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentTailOffset);

                    // Compute the size of the memory between the tail and the end.
                    size_t availableSpaceBetweenTailAndEnd = PointerArithmetic::Difference(tail, m_adjustedBufferPointerEnd);

                    // Check if the header can be fitted between current tail and the end of buffer
                    if(sizeof(CircleBufferHeader) < availableSpaceBetweenTailAndEnd) // Must be <, <= gives faulty results
                    {
                        //// Header can fit between the tail and the end of buffer

                        // Read metaheader
                        memcpy(o_header, tail, sizeof(CircleBufferHeader));

                        // Move tail
                        tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                        // Recompute the size of the memory between the new tail and the end.
                        availableSpaceBetweenTailAndEnd = PointerArithmetic::Difference(tail, m_adjustedBufferPointerEnd);

                        // Check if the data can be fitted between current tail and the end of buffer
                        if(o_header->packageSize <= availableSpaceBetweenTailAndEnd)
                        {
                            //// Data can fit between the tail and the end of buffer

                            // Copy data from current tail
                            memcpy(o_dataBuffer, tail, o_header->packageSize);

                            // Move header to after data
                            tail = PointerArithmetic::Addition(tail, o_header->packageSize);
                            success = true;
                        }
                        else
                        {
                            //// Data cannot fit between the tail and the end of buffer

                            // Move tail to start
                            tail = m_adjustedBufferPointerStart;

                            // Compute available space in the front of the buffer.
                            void* head = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentHeadOffset);
                            const size_t availableSpaceBetweenTailAndHead = PointerArithmetic::Difference(tail, head);

                            // Check if the data can fit in the front
                            if(o_header->packageSize <= availableSpaceBetweenTailAndHead)
                            {
                                //// Data can fit in the front

                                // Copy data from current_tail
                                memcpy(o_dataBuffer, tail, o_header->packageSize);

                                // Move header to after data
                                tail = PointerArithmetic::Addition(tail, o_header->packageSize);
                                success = true;
                            }
                            else
                            {
                                success = false;
                            }
                        }
                    }
                    else
                    {
                        //// Metaheader could not be placed in the end of the buffer

                        // Compute available space in the front of the buffer.
                        void* head = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_data->currentTailOffset);
                        const size_t availableSpaceInFront = PointerArithmetic::Difference(m_adjustedBufferPointerStart, head);

                        //
                        if(sizeof(CircleBufferHeader) < availableSpaceInFront)
                        {
                            // Move tail to start
                            tail = m_adjustedBufferPointerStart;

                            // Read metaheader
                            memcpy(o_header, tail, sizeof(CircleBufferHeader));

                            // Move tail
                            tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                            // Copy data from current_tail
                            memcpy(o_dataBuffer, tail, o_header->packageSize);

                            // Move header to after data
                            tail = PointerArithmetic::Addition(tail, o_header->packageSize);
                            success = true;
                        }
                    }

                    //
                    if(success)
                    {
                        m_data->currentTailOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, tail);
                        m_data->totalConsumedMemoryInBytes += (o_header->packageSize + sizeof(CircleBufferHeader));
                    }
                    return success;
                }

            protected:
                void AssertInitialize(const size_t& p_bufferSize)
                {
                    if(m_alreadyInitialized)
                    {
                        throw std::runtime_error("Already initialized.");
                    }

                    const uint32_t minimumSize = sizeof(CircleBufferHeader) + sizeof(ArbitraryStaticData);
                    if(p_bufferSize < minimumSize)
                    {
                        // TODORT better messages
                        const std::string errorMessage = std::string("Not enough space. Minimumsize is " + std::to_string(minimumSize) + " bytes.");
                        throw std::runtime_error(errorMessage);
                    }
                }

                void LockMetaData()
                {
                    if(m_metaDataMutex != nullptr)
                    {
                        m_metaDataMutex->lock();
                    }
                }

                void UnLockMetaData()
                {
                    if(m_metaDataMutex != nullptr)
                    {
                        m_metaDataMutex->unlock();
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

                size_t ComputeDiffBetweenProducedAndConsumed()
                {
                    const size_t consumed = m_data->totalConsumedMemoryInBytes;
                    const size_t produced = m_data->totalProducedMemoryInBytes;

                    if(consumed <= produced)
                    {
                        return produced - consumed;
                    }
                    else
                    {
                        const std::string message = std::string("Incorrect difference between consumption and production within the circlebuffer.");
                        throw std::runtime_error(message);
                    }
                }

                // TODORT can be moved to utility
                static bool CheckIfBothIsTrueOrBothIsFalse(bool a, bool b)
                {
                    const uint8_t value = static_cast<uint8_t>(a) + static_cast<uint8_t>(b);
                    return !static_cast<bool>(value % 2);
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
                IO::Mutex* m_metaDataMutex;
            };
        }
    }
}