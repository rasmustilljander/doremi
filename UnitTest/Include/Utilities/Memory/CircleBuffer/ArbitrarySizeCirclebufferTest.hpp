#pragma once
#include <gtest/gtest.h>
#include <Utility/Utilities/Include/Memory/Circlebuffer/ArbitrarySizeCirclebuffer.hpp>
#include <Utilities/Memory/TestStruct64.hpp>

using namespace Doremi::Utilities::Memory;

class ArbitrarySizeCirclebufferTest : public testing::Test
{
public:
    ArbitrarySizeCirclebufferTest() {}
    virtual ~ArbitrarySizeCirclebufferTest() {}

    ArbitrarySizeCirclebuffer* m_circleBuffer;

    void SetUp() override { m_circleBuffer = new ArbitrarySizeCirclebuffer(); }

    void TearDown() override { delete m_circleBuffer; }
};