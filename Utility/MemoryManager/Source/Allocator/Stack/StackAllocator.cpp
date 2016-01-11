#include <Allocator/Stack/StackAllocator.hpp>
#include <memory>
#include <HeaderBuilder/AllocationHeaderBuilder.hpp>


namespace Utility
{
    namespace MemoryManager
    {

        StackAllocator::StackAllocator() : m_top(nullptr) {}

        void StackAllocator::Initialize(const size_t& p_memorySize)
        {
            // Allocate the memory to use
            MemoryAllocator::Initialize(p_memorySize, 0);
            Clear();
        }

        uint8_t StackAllocator::ComputeAdjustment(void* p_adress, const uint8_t& p_alignment)
        {
            // p_alignment must be contained in the set of 2^x
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

        StackAllocator::~StackAllocator() { delete m_raw; }

        void* StackAllocator::Allocate(const size_t& p_memorySize, const uint8_t& p_alignment)
        {
            if(p_alignment % 2 != 0)
            {
                // TODO logging
                throw std::runtime_error("Alignment must be a tuple of 2.");
            }
            if(p_alignment == 0)
            {
                // TODO logging
                throw std::runtime_error("Alignment must be larger than 1.");
            }

            // Compute the desired memorysize
            const size_t expandedSize = p_memorySize + p_alignment;

            // Fetch the unaligned data.
            void* rawAdress = AllocateUnaligned(expandedSize);

            // Compute adjustment
            const uint8_t adjustment = ComputeAdjustment(rawAdress, p_alignment);

            // Final adress
            void* alignedAdress = reinterpret_cast<void*>(reinterpret_cast<size_t>(rawAdress) + adjustment);

            // Set adjustment metadata
            AllocationHeaderBuilder::SetAdjustment(alignedAdress, adjustment);

            return alignedAdress;
        }

        void* StackAllocator::AllocateUnaligned(const size_t& p_newMemorySize)
        {
            if(m_occupiedMemory + p_newMemorySize < m_totalMemory)
            {
                m_occupiedMemory += p_newMemorySize;
                void* returnAdress = m_top;
                m_top = reinterpret_cast<void*>(reinterpret_cast<size_t>(m_top) + p_newMemorySize);
                return returnAdress;
            }
            else
            {
                throw std::runtime_error("Error allocating - Not enough memory left in this stack.");
                // TODORT maybe allocate dynamically instead?
                //
                // TODORT log
            }
        }

        void StackAllocator::Clear()
        {
            m_top = m_raw;
            m_occupiedMemory = 0;
        }

        void StackAllocator::FreeToMarker(const MemoryMarker& p_marker)
        {
            if(p_marker.VerifyManager(this))
            {
                void* adress = p_marker.GetMarkerAdress();
                if(reinterpret_cast<size_t>(adress) > reinterpret_cast<size_t>(m_top))
                {
                    const uint8_t adjustment = AllocationHeaderBuilder::GetAdjustment(adress);
                    m_top = reinterpret_cast<void*>(reinterpret_cast<size_t>(adress) - adjustment);
                    m_occupiedMemory = reinterpret_cast<size_t>(m_top) - reinterpret_cast<size_t>(m_raw);
                }
                else
                { /* The marker is no longer valid, it points to memory that has already been released. */
                }
            }
            else
            {
                // TODO logging
                throw std::runtime_error("Wrong marker used.");
            }
        }

        MemoryMarker StackAllocator::GetMarker() { return MemoryMarker(m_top, this); }
    }
}