#include <Utilities/Memory/CircleBuffer/CircleBufferTest.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <memory>

TEST_F(CircleBufferTest, basicInit)
{
    m_circleBuffer->Initialize(100);
    SUCCEED();
}

TEST_F(CircleBufferTest, emptyConsume)
{
    m_circleBuffer->Initialize(100);

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();
    const bool res = m_circleBuffer->Consume(returnHeader, returnData);

    ASSERT_FALSE(res);
    delete returnHeader;
    delete returnData;
}

TEST_F(CircleBufferTest, toManyProduce)
{
    m_circleBuffer->Initialize(100);

    CircleBufferHeader sendHeader = CircleBufferHeader();
    TestStruct64 sendData = TestStruct64();
    bool res = m_circleBuffer->Produce(sendHeader, sendData);
    ASSERT_TRUE(res);
    res = m_circleBuffer->Produce(sendHeader, sendData);
    ASSERT_FALSE(res);
}

TEST_F(CircleBufferTest, basicProduceAndConsume)
{
    m_circleBuffer->Initialize(300);

    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    sendData.f1 = 4;
    m_circleBuffer->Produce(sendHeader, sendData);

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();
    m_circleBuffer->Consume(returnHeader, returnData);

    ASSERT_EQ(4, returnData->f1);
    ASSERT_EQ(64, returnHeader->packageSize);
    ASSERT_EQ(CircleBufferType(CircleBufferTypeEnum::TEXT).typeValue, CircleBufferType(returnHeader->packageType).typeValue);
    delete returnHeader;
    delete returnData;
}

TEST_F(CircleBufferTest, twoProduceAndConsume)
{
    m_circleBuffer->Initialize(300);

    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);

    TestStruct64 sendData = TestStruct64();
    sendData.f1 = 4;

    // Produce
    m_circleBuffer->Produce(sendHeader, sendData);
    sendData.f2 = 19;
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::COMMAND);
    m_circleBuffer->Produce(sendHeader, sendData);

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_EQ(4, returnData->f1);
    ASSERT_EQ(64, returnHeader->packageSize);
    ASSERT_EQ(CircleBufferType(CircleBufferTypeEnum::TEXT).typeValue, CircleBufferType(returnHeader->packageType).typeValue);

    m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_EQ(4, returnData->f1);
    ASSERT_EQ(19, returnData->f2);
    ASSERT_EQ(64, returnHeader->packageSize);
    ASSERT_EQ(CircleBufferType(CircleBufferTypeEnum::COMMAND).typeValue, CircleBufferType(returnHeader->packageType).typeValue);
    delete returnHeader;
    delete returnData;
}


TEST_F(CircleBufferTest, twoBuffersUsingDifferentPartOfSameExternalMemory)
{
    using namespace Doremi::Utilities;
    void* memory = malloc(400);
    m_circleBuffer->Initialize(memory, 200);

    CircleBuffer<TestStruct64>* otherBuffer = new CircleBuffer<TestStruct64>();
    otherBuffer->Initialize(PointerArithmetic::Addition(memory, 200), 200);

    // Build package
    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    bool res;

    //// Produce
    // Produce first buffer
    sendData.f1 = 4;
    res = m_circleBuffer->Produce(sendHeader, sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 19;
    res = m_circleBuffer->Produce(sendHeader, sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 78;
    res = m_circleBuffer->Produce(sendHeader, sendData);
    ASSERT_FALSE(res);

    // Produce second buffer
    sendData.f1 = 62;
    res = otherBuffer->Produce(sendHeader, sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 775;
    res = otherBuffer->Produce(sendHeader, sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 95;
    res = otherBuffer->Produce(sendHeader, sendData);
    ASSERT_FALSE(res);


    //// Consume
    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    // Consume first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(4, returnData->f1);

    res = m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(19, returnData->f1);

    res = m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_FALSE(res);

    // Consume second buffer
    res = otherBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(62, returnData->f1);

    res = otherBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(775, returnData->f1);

    res = otherBuffer->Consume(returnHeader, returnData);
    ASSERT_FALSE(res);

    free(memory);
    delete otherBuffer;
    delete returnHeader;
    delete returnData;
}


TEST_F(CircleBufferTest, twoBuffersPointingOnSameMemory)
{
    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    using namespace Doremi::Utilities;
    void* memory = malloc(200);
    m_circleBuffer->Initialize(memory, 200);

    CircleBuffer<TestStruct64>* otherBuffer = new CircleBuffer<TestStruct64>();
    otherBuffer->Initialize(memory, 200);

    // Build package
    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    bool res;

    // Produce with first buffer
    sendData.f1 = 98;
    res = m_circleBuffer->Produce(sendHeader, sendData); // 1
    ASSERT_TRUE(res);

    // Produce with second buffer
    sendData.f1 = 62;
    res = otherBuffer->Produce(sendHeader, sendData); // 2
    ASSERT_TRUE(res);

    // Produce with first buffer
    sendData.f1 = 4;
    res = m_circleBuffer->Produce(sendHeader, sendData); // 2
    ASSERT_FALSE(res);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(98, returnData->f1);

    // Produce with first buffer
    sendData.f1 = 33;
    res = m_circleBuffer->Produce(sendHeader, sendData); // 2
    ASSERT_TRUE(res);

    // Consume with second  buffer
    res = otherBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(62, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_TRUE(res);
    ASSERT_EQ(33, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_FALSE(res);

    free(memory);
    delete otherBuffer;
    delete returnHeader;
    delete returnData;
}