#pragma once
#include <gtest/gtest.h>
#include <Utilities/Memory/TestStruct64.hpp>
#include <Utility/Utilities/Include/Memory/Pool/FixedSizePoolAllocator.hpp>

using namespace Doremi::Utilities::Memory;
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