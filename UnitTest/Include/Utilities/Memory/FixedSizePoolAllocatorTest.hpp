#pragma once
#include <gtest/gtest.h>
#include <Utilities/Memory/TestStruct64.hpp>
#include <Utility/MemoryManager/Include/Allocator/Pool/FixedSizePoolAllocator.hpp>

using namespace Utility::MemoryManager;
class FixedSizePoolAllocatorTest : public testing::Test
{
public:
    FixedSizePoolAllocatorTest() {}
    virtual ~FixedSizePoolAllocatorTest() {}

    FixedSizePoolAllocator<TestStruct64>* m_poolAllocator;

    void SetUp() override { m_poolAllocator = new FixedSizePoolAllocator<TestStruct64>(); }

    void TearDown() override
    {
        m_poolAllocator->Clear();
        delete m_poolAllocator;
    }
};