#include <Utilities/Memory/StackAllocatorTest.hpp>

TEST_F(StackAllocatorTest, basicInitialization)
{
    m_stackAllocator->Initialize(1024);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().free, 1024);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().occupied, 0);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().total, 1024);
}

TEST_F(StackAllocatorTest, allocationAlignment2)
{
    m_stackAllocator->Initialize(1024);
    m_stackAllocator->Allocate<TestStruct64>(2);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().free, 958);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().occupied, 66);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().total, 1024);
}

TEST_F(StackAllocatorTest, allocationAlignment4)
{
    m_stackAllocator->Initialize(1024);
    m_stackAllocator->Allocate<TestStruct64>(4);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().free, 956);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().occupied, 68);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().total, 1024);
}

TEST_F(StackAllocatorTest, AllocationFaultyOddAlignment)
{
    m_stackAllocator->Initialize(1024);
    ASSERT_THROW(m_stackAllocator->Allocate<TestStruct64>(3), std::runtime_error);
}
