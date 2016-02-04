#include <Utilities/Memory/FixedSizePoolAllocatorTest.hpp>

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

TEST_F(FixedSizePoolAllocatorTest, basicAllocation)
{
    m_poolAllocator->Initialize(64, 0);
    TestStruct64* test = m_poolAllocator->Allocate();

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 4032);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 64);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4096);
    ASSERT_TRUE(test != nullptr);
}

TEST_F(FixedSizePoolAllocatorTest, basicAllocationLocation)
{
    m_poolAllocator->Initialize(64, 0);
    TestStruct64* first = m_poolAllocator->Allocate();
    TestStruct64* second = m_poolAllocator->Allocate();

    ASSERT_NE(first, second);
    ASSERT_TRUE(first < second);
}

template <typename T> void loopAllocate(FixedSizePoolAllocator<T>* p_allocator, const size_t& p_count)
{
    T* prev = p_allocator->Allocate();

    for(size_t i = 0; i < p_count - 1; ++i)
    {
        T* current = p_allocator->Allocate();
        ASSERT_TRUE(prev < current);
        ASSERT_TRUE(current != nullptr);
    }
}

TEST_F(FixedSizePoolAllocatorTest, halfAllocation)
{
    m_poolAllocator->Initialize(64, 0);
    loopAllocate<TestStruct64>(m_poolAllocator, 32);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 2048);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 2048);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4096);
}

TEST_F(FixedSizePoolAllocatorTest, fullAllocation)
{
    m_poolAllocator->Initialize(64, 0);
    loopAllocate<TestStruct64>(m_poolAllocator, 64);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 4096);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4096);
}

TEST_F(FixedSizePoolAllocatorTest, overAllocation)
{
    m_poolAllocator->Initialize(64, 0);
    ASSERT_THROW(loopAllocate<TestStruct64>(m_poolAllocator, 65), std::runtime_error);
}

TEST_F(FixedSizePoolAllocatorTest, largeAllocation)
{
    m_poolAllocator->Initialize(1562500, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 100000000);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);

    loopAllocate<TestStruct64>(m_poolAllocator, 1562500);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 100000000);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);
}

TEST_F(FixedSizePoolAllocatorTest, basicFree)
{
    m_poolAllocator->Initialize(64, 0);

    TestStruct64* pointer = m_poolAllocator->Allocate();
    loopAllocate<TestStruct64>(m_poolAllocator, 63);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 4096);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4096);

    m_poolAllocator->Free(pointer);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 64);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 4032);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 4096);
}

TEST_F(FixedSizePoolAllocatorTest, freeLogicOnLargeSet)
{
    m_poolAllocator->Initialize(1562500, 0);
    TestStruct64* first = m_poolAllocator->Allocate();
    loopAllocate<TestStruct64>(m_poolAllocator, 781250 - 2); // Allocate about half
    TestStruct64* mid = m_poolAllocator->Allocate();
    loopAllocate<TestStruct64>(m_poolAllocator, 781250 - 1); // Allocate the resthalf
    TestStruct64* end = m_poolAllocator->Allocate();

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 100000000);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);

    m_poolAllocator->Free(mid);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 64);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 99999936);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);

    TestStruct64* newest = m_poolAllocator->Allocate();
    ASSERT_TRUE(first < newest);
    ASSERT_TRUE(newest < end);
    ASSERT_EQ(newest, mid);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 100000000);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);
}

TEST_F(FixedSizePoolAllocatorTest, advancedFreeLogicOnLargeSet)
{
    m_poolAllocator->Initialize(1562500, 0);
    TestStruct64* first = m_poolAllocator->Allocate();
    loopAllocate<TestStruct64>(m_poolAllocator, 781250 - 11); // Allocate about half

    TestStruct64* midPointers[10];
    for(size_t i = 0; i < 10; ++i)
    {
        midPointers[i] = m_poolAllocator->Allocate();
    }

    loopAllocate<TestStruct64>(m_poolAllocator, 781250 - 10); // Allocate the resthalf
    TestStruct64* end[10];
    for(size_t i = 0; i < 10; ++i)
    {
        end[i] = m_poolAllocator->Allocate();
    }

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 100000000);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);

    m_poolAllocator->Free(end[5]);
    m_poolAllocator->Free(end[4]);
    m_poolAllocator->Free(end[6]);

    ASSERT_EQ(192, m_poolAllocator->GetMemorySpecification().free);
    ASSERT_EQ(99999808, m_poolAllocator->GetMemorySpecification().occupied);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);

    TestStruct64* newest = m_poolAllocator->Allocate();

    ASSERT_TRUE(first < newest);
    ASSERT_TRUE(newest < end[9]);
    ASSERT_EQ(newest, end[6]);

    newest = m_poolAllocator->Allocate();

    ASSERT_TRUE(first < newest);
    ASSERT_TRUE(newest < end[9]);
    ASSERT_EQ(newest, end[4]);

    newest = m_poolAllocator->Allocate();

    ASSERT_TRUE(first < newest);
    ASSERT_TRUE(newest < end[9]);
    ASSERT_EQ(newest, end[5]);

    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().free, 0);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().occupied, 100000000);
    ASSERT_EQ(m_poolAllocator->GetMemorySpecification().total, 100000000);
}