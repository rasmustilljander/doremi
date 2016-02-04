#include <Memory/MemoryAllocator.hpp>
#include <memory>

namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            MemoryAllocator::MemoryAllocator()
                : m_occupiedMemory(0), m_totalMemory(0), m_memoryStartRaw(nullptr), m_memoryEndRaw(nullptr), m_memoryStartAligned(nullptr), m_alignment(0), m_adjustment(0)
            {
            }

            MemoryAllocator::~MemoryAllocator() { std::free(m_memoryStartRaw); }


            void MemoryAllocator::Initialize(const size_t& p_memorySize, const uint8_t& p_alignment)
            {
                m_alignment = p_alignment;
                m_totalMemory = p_memorySize + m_alignment;
                AllocateFirstTime();
            }

            MemorySpecification MemoryAllocator::GetMemorySpecification()
            {
                return MemorySpecification(m_totalMemory - m_occupiedMemory, m_totalMemory, m_occupiedMemory);
            }

            void MemoryAllocator::AllocateFirstTime()
            {
                // Allocate a chunk of memory and save the raw pointer
                m_memoryStartRaw = std::malloc(m_totalMemory);

                // Adress to end of memory chunk
                m_memoryEndRaw = reinterpret_cast<void*>(reinterpret_cast<size_t>(m_memoryStartRaw) + m_totalMemory);

                // Set entire chunk to zero
                memset(m_memoryStartRaw, 0, m_totalMemory); // TODORT could be removed in the future

                // Compute the adjustment basd on alignment
                m_adjustment = ComputeAdjustment(m_memoryStartRaw, m_alignment);

                // Get the aligned pointer to the memory chunk
                m_memoryStartAligned = reinterpret_cast<void*>(reinterpret_cast<size_t>(m_memoryStartRaw) + m_adjustment);
            }

            uint8_t MemoryAllocator::ComputeAdjustment(void* p_adress, const uint8_t& p_alignment)
            {
                // p_alignment must be contained in the set of 2^x
                if(p_alignment % 2 == 0 && p_alignment > 1)
                {
                    // Mask must be subtracted by 1, always.
                    // 16 = 0x10 = 0001 0000
                    // 15 = 0xF = 0000 1111
                    //   XXXX XXXX
                    // & 0000 1111
                    // Which gives a misalignment between 0 and 2^x - 1
                    // Thus adjustment can never be less than 1.
                    const uint8_t mask = p_alignment - 1;
                    const uint8_t misalignment = (reinterpret_cast<size_t>(p_adress) & mask);
                    const uint8_t adjustment = p_alignment - misalignment;

                    return adjustment;
                }
                return 0;
            }

            bool MemoryAllocator::AssertAdresstInside(void* p_adressToAssert) const
            {
                if(p_adressToAssert >= GetAdressStartRaw() && p_adressToAssert < GetAdressEndRaw())
                {
                    return true; // Inside
                }
                return false; // Outside
            }
        }
    }
}