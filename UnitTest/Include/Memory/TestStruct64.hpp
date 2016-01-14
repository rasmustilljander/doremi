#pragma once
#include <gtest/gtest.h>

struct TestStruct64
{
    float f1 = 1;
    float f2 = f1 + f1;
    float f3 = f2 + f2;
    float f4 = f3 + f3;
    float f5 = f4 + f4;
    float f6 = f5 + f5;
    float f7 = f6 + f6;
    float f8 = f7 + f7;
    float f9 = f8 + f8;
    float f10 = f9 + f9;
    float f11 = f10 + f10;
    float f12 = f11 + f11;
    float f13 = f12 + f12;
    float f14 = f13 + f13;
    float f15 = f14 + f14;
    float f16 = f15 + f15;
};

class TestStruct64Test : public testing::Test
{
public:
    TestStruct64Test() {}
    virtual ~TestStruct64Test() {}

    TestStruct64 testStruct;

    void SetUp() override {}

    void TearDown() override {}
};
