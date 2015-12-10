#pragma once

namespace Utility
{
    namespace Memory
    {
        template <typename T> StackAllocator::StackAllocator() {}

        template <typename T> virtual StackAllocator::~StackAllocator() {}
    }
}