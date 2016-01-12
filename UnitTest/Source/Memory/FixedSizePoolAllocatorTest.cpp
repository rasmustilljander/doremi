#include <Memory/FixedSizePoolAllocatorTest.hpp>

TEST_F(FixedSizePoolAllocatorTest, basicInitialization)
{
    m_poolAllocator->Initialize(64, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 4096);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4096);
}

TEST_F(FixedSizePoolAllocatorTest, initAlignment2)
{
    m_poolAllocator->Initialize(64, 2);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 4098);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4098);
}

TEST_F(FixedSizePoolAllocatorTest, initAlignment4)
{
    m_poolAllocator->Initialize(64, 4);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 4100);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4100);
}

/*
TEST_F(FixedSizePoolAllocatorTest, AllocationFaultyOddAlignment)
{
    ASSERT_THROW(m_poolAllocator->Initialize(64, 3), std::runtime_error);
 //   ASSERT_THROW(m_stackAllocator->Allocate<TestStruct64>(3), std::runtime_error);
}
*/