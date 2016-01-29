#include <gtest/gtest.h>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/InterpolationHandler.hpp>

using namespace Doremi::Core;

TEST(ExtrapolationTest, ExtrapolateBasic)
{
    TransformComponentSnapshotPrevious* prev = GetComponent<TransformComponentSnapshotPrevious>(0);
    TransformComponentSnapshotNext* next = GetComponent<TransformComponentSnapshotNext>(0);

    prev->framesToNext = 1;
    next->framesToNextCounter = 1;
    prev->position.x = 0.0f;
    next->position.x = 1.0f;


    TransformComponentNext nextReal = ExtrapolateTransform(prev, next, 1.0f);

    ASSERT_EQ(2.0f, nextReal.position.x);
}

TEST(ExtrapolationTest, ExtrapolateNegativBasic)
{
    TransformComponentSnapshotPrevious* prev = GetComponent<TransformComponentSnapshotPrevious>(0);
    TransformComponentSnapshotNext* next = GetComponent<TransformComponentSnapshotNext>(0);

    prev->framesToNext = 1;
    next->framesToNextCounter = 1;
    prev->position.x = 2.0f;
    next->position.x = 1.0f;


    TransformComponentNext nextReal = ExtrapolateTransform(prev, next, 1.0f);

    ASSERT_EQ(0.0f, nextReal.position.x);
}

TEST(ExtrapolationTest, ExtrapolateWithHalfLengthsStep)
{
    TransformComponentSnapshotPrevious* prev = GetComponent<TransformComponentSnapshotPrevious>(0);
    TransformComponentSnapshotNext* next = GetComponent<TransformComponentSnapshotNext>(0);

    prev->framesToNext = 4;
    next->framesToNextCounter = 1;
    prev->position.x = 0.0f;
    next->position.x = 1.0f;


    TransformComponentNext nextReal = ExtrapolateTransform(prev, next, 1.0f);

    ASSERT_EQ(1.25f, nextReal.position.x);
}

TEST(ExtrapolationTest, ExtrapolateWithIncreaedLength)
{
    TransformComponentSnapshotPrevious* prev = GetComponent<TransformComponentSnapshotPrevious>(0);
    TransformComponentSnapshotNext* next = GetComponent<TransformComponentSnapshotNext>(0);

    prev->framesToNext = 1;
    next->framesToNextCounter = 1;
    prev->position.x = 0.0f;
    next->position.x = 1.0f;


    TransformComponentNext nextReal = ExtrapolateTransform(prev, next, 4.0f);

    ASSERT_EQ(5.0f, nextReal.position.x);
}

TEST(ExtrapolationTest, ExtrapolateWithIncreaedLengthAndHalfLengthsStep)
{
    TransformComponentSnapshotPrevious* prev = GetComponent<TransformComponentSnapshotPrevious>(0);
    TransformComponentSnapshotNext* next = GetComponent<TransformComponentSnapshotNext>(0);

    prev->framesToNext = 4;
    next->framesToNextCounter = 1;
    prev->position.x = 0.0f;
    next->position.x = 1.0f;


    TransformComponentNext nextReal = ExtrapolateTransform(prev, next, 8.0f);

    ASSERT_EQ(3.0f, nextReal.position.x);
}

// TEST(ExtrapolationTest, ExtrapolateWithShelfSwap)
//{
//    TransformComponentSnapshotPrevious* prev = GetComponent<TransformComponentSnapshotPrevious>(0);
//    TransformComponentSnapshotNext* next = GetComponent<TransformComponentSnapshotNext>(0);
//
//
//    prev->position.x = 0.0f;
//    next->position.x = 1.0f;
//
//    *next = ExtrapolateTransform(prev, next, 2.0f);
//
//
//
//    *next = ExtrapolateTransform(old, prev, 2.0f);
//
//
//    ASSERT_EQ(2.0f, next->position.x);
//}