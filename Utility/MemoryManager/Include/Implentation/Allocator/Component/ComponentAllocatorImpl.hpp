#pragma once

namespace Utility
{
    namespace Memory
    {
        template <typename T> ComponentAllocator::ComponentAllocator() {}

        template <typename T> virtual ComponentAllocator::~ComponentAllocator() {}
    }
}