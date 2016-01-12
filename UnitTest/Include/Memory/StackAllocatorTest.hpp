#pragma once
#include <gtest/gtest.h>
#include <Utility/MemoryManager/Include/Allocator/Stack/StackAllocator.hpp>
#include <Memory/TestStruct64.hpp>

using namespace Utility::MemoryManager;
class StackAllocatorTest : public testing::Test
{
    public:
    StackAllocatorTest() {}
    virtual ~StackAllocatorTest() {}

    StackAllocator* m_stackAllocator;

    void SetUp() override { m_stackAllocator = new StackAllocator(); }

    void TearDown() override
    {
        m_stackAllocator->Clear();
        delete m_stackAllocator;
    }
};