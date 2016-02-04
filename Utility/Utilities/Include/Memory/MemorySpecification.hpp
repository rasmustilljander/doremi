#pragma once
#include <cstdint>
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            struct MemorySpecification
            {
                size_t free;
                size_t total;
                size_t occupied;
                MemorySpecification(const size_t& p_free, const size_t& p_total, const size_t& p_occupied)
                    : free(p_free), total(p_total), occupied(p_occupied)
                {
                }
            };
        }
    }
}