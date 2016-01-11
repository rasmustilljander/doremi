#include <Memory/StackAllocatorTest.hpp>

TEST_F(StackAllocatorTest, basicInitialization)
{
    m_stackAllocator->Initialize(1024);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().free, 1024);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().occupied, 0);
    ASSERT_EQ(m_stackAllocator->GetMemorySpecification().total, 1024);
}

TEST_F(StackAllocatorTest, basic64Struct) { ASSERT_EQ(sizeof(TestStruct64), 64); }

TEST_F(StackAllocatorTest, basic64StructContains)
{
    TestStruct64 a;
    ASSERT_FLOAT_EQ(a.f1, 1.0f);
    ASSERT_FLOAT_EQ(a.f2, 2.0f);
    ASSERT_FLOAT_EQ(a.f3, 4.0f);
    ASSERT_FLOAT_EQ(a.f4, 8.0f);
    ASSERT_FLOAT_EQ(a.f5, 16.0f);
    ASSERT_FLOAT_EQ(a.f6, 32.0f);
    ASSERT_FLOAT_EQ(a.f7, 64.0f);
    ASSERT_FLOAT_EQ(a.f8, 128.0f);
    ASSERT_FLOAT_EQ(a.f9, 256.0f);
    ASSERT_FLOAT_EQ(a.f10, 512.0f);
    ASSERT_FLOAT_EQ(a.f11, 1024.0f);
    ASSERT_FLOAT_EQ(a.f12, 2048.0f);
    ASSERT_FLOAT_EQ(a.f13, 4096.0f);
    ASSERT_FLOAT_EQ(a.f14, 8192.0f);
    ASSERT_FLOAT_EQ(a.f15, 16384.0f);
    ASSERT_FLOAT_EQ(a.f16, 32768.0f);
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
