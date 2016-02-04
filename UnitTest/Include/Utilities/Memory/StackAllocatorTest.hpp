#pragma once
#include <gtest/gtest.h>
#include <Utility/Utilities/Include/Memory/Stack/StackAllocator.hpp>
#include <Utilities/Memory/TestStruct64.hpp>

using namespace Doremi::Utilities::Memory;
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