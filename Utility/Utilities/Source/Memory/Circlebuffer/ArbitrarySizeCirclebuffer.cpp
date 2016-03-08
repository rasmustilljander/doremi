#pragma once
#include <Memory/Circlebuffer/ArbitrarySizeCirclebuffer.hpp>
#include <Memory/Circlebuffer/CircleBufferHeader.hpp>
#include <Memory/Circlebuffer/ArbitraryStaticData.hpp>
#include <string>
#include <iostream>

namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            ArbitrarySizeCirclebuffer::ArbitrarySizeCirclebuffer()
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

            ArbitrarySizeCirclebuffer::~ArbitrarySizeCirclebuffer()
            {
                if(m_internalMemoryManagement)
                {
                    free(m_rawBufferPointerStart);
                }
                if(m_metaDataMutex)
                {
                    m_metaDataMutex->unlock();
                }
            }

            void ArbitrarySizeCirclebuffer::Initialize(const uint32_t& p_bufferSize, IO::Mutex* p_sharedMemoryMutex)
            {
                AssertInitialize(p_bufferSize);
                m_rawBufferSize = p_bufferSize;
                m_rawBufferPointerStart = malloc(m_rawBufferSize);
                m_metaDataMutex = p_sharedMemoryMutex;
                SetupVariables();
            }

            void ArbitrarySizeCirclebuffer::Initialize(void* const p_preAllocatedBuffer, const uint32_t& p_bufferSize, IO::Mutex* p_sharedMemoryMutex)
            {
                AssertInitialize(p_bufferSize);
                m_rawBufferSize = p_bufferSize;
                m_rawBufferPointerStart = p_preAllocatedBuffer;
                m_internalMemoryManagement = false;
                m_metaDataMutex = p_sharedMemoryMutex;
                SetupVariables();
            }

            bool ArbitrarySizeCirclebuffer::Produce(const CircleBufferHeader& p_Header, const void* const p_data)
            {
                // Internal lockage
                std::lock_guard<std::mutex> lock(m_produceLock);

                // Fetch variables from shared memory
                // These variables must be read at the same time :s
                const size_t tailOffset = m_data->currentTailOffset;
                const size_t headOffset = m_data->currentHeadOffset;

                // Compute variables based on shared memory
                void* tail = PointerArithmetic::Addition(m_adjustedBufferPointerStart, tailOffset); // Compute the location of the current tail
                void* head = PointerArithmetic::Addition(m_adjustedBufferPointerStart, headOffset); // Compute the location of the head
                const intmax_t diff = PointerArithmetic::Difference(tail, head); // Compute the difference between the two

                intmax_t part1, part2 = 0;
                // Compute currently available space
                if(diff == 0)
                {
                    //// Buffer is empty
                    //// Use the diff between current head and end + the diff between the start and tail
                    part1 = PointerArithmetic::Difference(head, m_adjustedBufferPointerEnd);
                    part2 = PointerArithmetic::Difference(m_adjustedBufferPointerStart, tail);
                }
                else if(diff > 0)
                {
                    //// Head is to the "right" of the tail, use the diff between current head and end + the diff between the start and tail
                    part1 = PointerArithmetic::Difference(head, m_adjustedBufferPointerEnd);
                    part2 = PointerArithmetic::Difference(m_adjustedBufferPointerStart, tail);
                    // TODOXX TODORT cast uint32_t intman_t ??
                }
                else
                {
                    //// Head is to the "left" of the tail, use the diff between current head and end
                    part1 = diff * -1;
                }
                uint32_t currentlyAvailableSpace = part1 + part2;

                // Check so that the data can fit within the buffer
                const uint32_t requestedSize = sizeof(CircleBufferHeader) + p_Header.packageSize;
                if(currentlyAvailableSpace < requestedSize)
                {
#ifdef _DEBUG
                    printf("Not enough space in circlebuffer. Size of produce request %d. This buffer only have access to %d", requestedSize, currentlyAvailableSpace);
#endif
                    return false;
                }

                bool metaHeaderInPart1, dataPackageInPart1;

                // Check if the metaheader can be fitted in the end
                if (sizeof(CircleBufferHeader) <= part1)
                {
                    //// Atleast metaheader can be fitted in the first part
                    metaHeaderInPart1 = true;

                    // Check if the datapackage can be fitted in the first part after metaheader
                    if (p_Header.packageSize <= part1 - sizeof(CircleBufferHeader))
                    {
                        dataPackageInPart1 = true;
                    }
                    
                    // Check if datapackage can be fitted in the second part
                    else if(p_Header.packageSize <= part2)
                    {
                        dataPackageInPart1 = false;
                    }
                    else
                    {
#ifdef _DEBUG
                        printf("Metaheader fits within the first part, but datapackage does not fit anywhere.");
#endif
                        return false;
                    }
                }
                // Check if metaheader can be fitted within the second part
                else if(sizeof(CircleBufferHeader) <= part2)
                {
                    //// Atleast metaheader can be fitted in the second part
                    metaHeaderInPart1 = false;

                    // Check if the datapackage can be fitted in the second part after metaheader
                    if (p_Header.packageSize <= part2 - sizeof(CircleBufferHeader))
                    {
                        dataPackageInPart1 = false;
                    }
                    else
                    {
#ifdef _DEBUG
                        printf("Metaheader fits within second part, but not packagedata.");
#endif
                        return false;
                    }
                }
                else
                {
#ifdef _DEBUG
                    printf("Nothing fits");
#endif
                    return false;
                }

                void* locationOfMetaHeader;
                void* locationOfDataPackage;

                // Check if both is false or if both is true
                if(metaHeaderInPart1 == dataPackageInPart1)
                {
                    //// Both is true or both is false. Means that they are placed together, metadataheader first followed by the data.

                    // Check if they're placed in the end
                    if(metaHeaderInPart1)
                    {
                        //// Both are placed in the end
                        locationOfMetaHeader = head;
                    }
                    else
                    {
                        //// Both are placed infront
                        locationOfMetaHeader = m_adjustedBufferPointerStart;
                    }
                    // data is always right after
                    locationOfDataPackage = PointerArithmetic::Addition(locationOfMetaHeader, sizeof(CircleBufferHeader));
                }
                else
                {
                    //// Means that the metadataheader is placed last and the data in front
                    locationOfMetaHeader = head;
                    locationOfDataPackage = m_adjustedBufferPointerStart;
                }

                //// Helpdata complete, time to write
                // Write header
                memcpy(locationOfMetaHeader, &p_Header, sizeof(CircleBufferHeader));

                // Write data
                memcpy(locationOfDataPackage, p_data, p_Header.packageSize);

                // Update shared head
                head = PointerArithmetic::Addition(locationOfDataPackage, p_Header.packageSize);

                m_data->currentHeadOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, head);
                intmax_t diff2 = PointerArithmetic::Difference(tail, head);
                return true;
            }

            bool ArbitrarySizeCirclebuffer::Consume(CircleBufferHeader*& o_header, void* o_dataBuffer, const uint32_t& p_outbufferSize)
            {
                // Internal lockage
                std::lock_guard<std::mutex> lock(m_consumeLock);
                bool success = false;

                // Fetch variables from shared memory
                // These variables must be read at the same time :s
                const size_t tailOffset = m_data->currentTailOffset;
                const size_t headOffset = m_data->currentHeadOffset;

                // Compute variables based on shared memory
                void* tail = PointerArithmetic::Addition(m_adjustedBufferPointerStart, tailOffset); // Compute the location of the current tail
                void* head = PointerArithmetic::Addition(m_adjustedBufferPointerStart, headOffset); // Compute the location of the head
                const intmax_t diff = PointerArithmetic::Difference(tail, head); // Compute the difference between the two

                // Compute currently occupied space
                intmax_t part1 = 0;
                intmax_t part2 = 0;
                if(diff == 0)
                {
                    //// Buffer is empty
                    // Nothing to consume
                    return false;
                }
                else if(diff > 0)
                {
                    //// Head is to the "right" of the tail, use the diff between tail and the current head
                    part1 = PointerArithmetic::Difference(tail, head);
                }
                else
                {
                    //// Head is to the "left" of the tail, use the diff between current tail and end + the diff between start and head
                    part1 = PointerArithmetic::Difference(tail, m_adjustedBufferPointerEnd);
                    part2 = PointerArithmetic::Difference(m_adjustedBufferPointerStart, head);
                }

                // Check if a header could have been placed in the first part
                if(sizeof(CircleBufferHeader) <= part1)
                {
                    //// Metaheader can fit within the first part

                    // Read metaheader
                    memcpy(o_header, tail, sizeof(CircleBufferHeader));

                    // Check if the datapackage could have been placed within the first part after the metaheader
                    if(o_header->packageSize <= part1 - sizeof(CircleBufferHeader))
                    {
                        //// Data can fit between the tail and the end of buffer

                        // Move tail
                        tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                        // Copy data from current tail
                        memcpy(o_dataBuffer, tail, o_header->packageSize);

                        // Move header to after data
                        tail = PointerArithmetic::Addition(tail, o_header->packageSize);
                        success = true;
                    }
                    else
                    {
                        //// The datapackage could have been placed within the first part after the metaheader

                        // Check if the datapackage can be fitted in the front
                        if (o_header->packageSize <= part2)
                        {
                            //// Data can fit in the front
                            // Move tail to start
                            tail = m_adjustedBufferPointerStart;

                            // Copy data from current tail
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

                    //Check if metaheader can be placed in the front
                    if(sizeof(CircleBufferHeader) < part2)
                    {
                        // Move tail to start
                        tail = m_adjustedBufferPointerStart;

                        // Read metaheader
                        memcpy(o_header, tail, sizeof(CircleBufferHeader));

                        // Check if the datapackage can be placed after the metaheader
                        if (o_header->packageSize < (part2 - sizeof(CircleBufferHeader)))
                        {
                            // Move tail
                            tail = PointerArithmetic::Addition(tail, sizeof(CircleBufferHeader));

                            // Copy data from current_tail
                            memcpy(o_dataBuffer, tail, o_header->packageSize);

                            // Move header to after data
                            tail = PointerArithmetic::Addition(tail, o_header->packageSize);
                            success = true;
                        }
                    }
                }

                if(success)
                {
                    m_data->currentTailOffset = PointerArithmetic::Difference(m_adjustedBufferPointerStart, tail);
                }
                return success;
            }

            void ArbitrarySizeCirclebuffer::AssertInitialize(const uint32_t& p_bufferSize)
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

            void ArbitrarySizeCirclebuffer::LockMetaData()
            {
                if(m_metaDataMutex != nullptr)
                {
                    m_metaDataMutex->lock();
                }
            }

            void ArbitrarySizeCirclebuffer::UnLockMetaData()
            {
                if(m_metaDataMutex != nullptr)
                {
                    m_metaDataMutex->unlock();
                }
            }
            void ArbitrarySizeCirclebuffer::ComputeAdjustments()
            {
                m_adjustedBufferSize = m_rawBufferSize - sizeof(ArbitraryStaticData);

                m_adjustedBufferPointerStart = PointerArithmetic::Addition(m_rawBufferPointerStart, sizeof(ArbitraryStaticData));
                m_adjustedBufferPointerEnd = PointerArithmetic::Addition(m_adjustedBufferPointerStart, m_adjustedBufferSize);
            }

            void ArbitrarySizeCirclebuffer::SetupVariables()
            {
                m_alreadyInitialized = true;
                m_rawBufferPointerEnd = PointerArithmetic::Addition(m_rawBufferPointerStart, m_rawBufferSize);
                ComputeAdjustments();
                m_data = static_cast<ArbitraryStaticData*>(m_rawBufferPointerStart);

                if(m_metaDataMutex != nullptr)
                {
                    if(m_metaDataMutex->try_lock())
                    {
                        printf("This process is the owner of the metadata lock for the circlebuffer.\n");
                        ResetMetaData();
                    }
                }
                else
                {
                    ResetMetaData();
                }
            }

            void ArbitrarySizeCirclebuffer::ResetMetaData()
            {
                *m_data = ArbitraryStaticData();
                m_data->currentTailOffset = 0;
                m_data->currentHeadOffset = 0;
            }
        }
    }
}