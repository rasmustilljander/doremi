#include <gtest/gtest.h>
#include <Doremi/Core/Include/SequenceMath.hpp>

using namespace Doremi::Core;

TEST(SqeuenceMathTest, subtractionBasic)
{
    int32_t returnValue = sequence_difference(50, 25, 255);

    ASSERT_EQ(25, returnValue);
}

TEST(SqeuenceMathTest, subtractionOverMax)
{
    int32_t returnValue = sequence_difference(5, 245, 255);

    ASSERT_EQ(16, returnValue);
}

TEST(SqeuenceMathTest, subtractionZero)
{
    int32_t returnValue = sequence_difference(0, 0, 255);

    ASSERT_EQ(0, returnValue);
}

TEST(SqeuenceMathTest, subtractionMax)
{
    int32_t returnValue = sequence_difference(255, 255, 255);

    ASSERT_EQ(0, returnValue);
}

TEST(SqeuenceMathTest, subtractionNegative)
{
    int32_t returnValue = sequence_difference(200, 205, 255);

    ASSERT_EQ(-5, returnValue);
}

TEST(SqeuenceMathTest, subtractionNegativeOverMax)
{
    int32_t returnValue = sequence_difference(245, 5, 255);

    ASSERT_EQ(-16, returnValue);
}

TEST(SequenceMathTest, moreRecentBasicTrue) { ASSERT_TRUE(sequence_more_recent(50, 40, 255)); }

TEST(SequenceMathTest, moreRecentBasicFalse) { ASSERT_FALSE(sequence_more_recent(40, 50, 255)); }

TEST(SequenceMathTest, moreRecentEqual) { ASSERT_FALSE(sequence_more_recent(0, 0, 255)); }

TEST(SequenceMathTest, moreRecentOverMaxTrue) { ASSERT_TRUE(sequence_more_recent(5, 245, 255)); }

TEST(SequenceMathTest, moreRecentOverMaxFalse) { ASSERT_FALSE(sequence_more_recent(245, 5, 255)); }