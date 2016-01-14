#pragma once
#include <gtest/gtest.h>
#include <Doremi/Core/Include/Helper/ProximityChecker.hpp>

class ProximityHelperTest : public testing::Test, public Doremi::Core::ProximityChecker
{
public:
    ProximityHelperTest();
    ~ProximityHelperTest();
    void SetUp();
    void TearDown();
};