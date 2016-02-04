#include <Utilities/Memory/TestStruct64.hpp>

TEST_F(TestStruct64Test, basic64Struct) { ASSERT_EQ(sizeof(TestStruct64), 64); }

TEST_F(TestStruct64Test, basic64StructContains)
{
    ASSERT_FLOAT_EQ(testStruct.f1, 1.0f);
    ASSERT_FLOAT_EQ(testStruct.f2, 2.0f);
    ASSERT_FLOAT_EQ(testStruct.f3, 4.0f);
    ASSERT_FLOAT_EQ(testStruct.f4, 8.0f);
    ASSERT_FLOAT_EQ(testStruct.f5, 16.0f);
    ASSERT_FLOAT_EQ(testStruct.f6, 32.0f);
    ASSERT_FLOAT_EQ(testStruct.f7, 64.0f);
    ASSERT_FLOAT_EQ(testStruct.f8, 128.0f);
    ASSERT_FLOAT_EQ(testStruct.f9, 256.0f);
    ASSERT_FLOAT_EQ(testStruct.f10, 512.0f);
    ASSERT_FLOAT_EQ(testStruct.f11, 1024.0f);
    ASSERT_FLOAT_EQ(testStruct.f12, 2048.0f);
    ASSERT_FLOAT_EQ(testStruct.f13, 4096.0f);
    ASSERT_FLOAT_EQ(testStruct.f14, 8192.0f);
    ASSERT_FLOAT_EQ(testStruct.f15, 16384.0f);
    ASSERT_FLOAT_EQ(testStruct.f16, 32768.0f);
}
