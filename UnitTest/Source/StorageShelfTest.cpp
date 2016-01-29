#include <gtest/gtest.h>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>

using namespace Doremi::Core;

// Can't test this now as the component tested got replaced
// TEST(StorageShelfTest, SwapTest)
//{
//    GetComponent<TransformComponentPrevious>(0)->position.x = 1.0f;
//    GetComponent<TransformComponentNext>(0)->position.x = 2.0f;
//    GetComponent<TransformComponentOld>(0)->position.x = 3.0f;
//
//    RotateShelfs<TransformComponentPrevious, TransformComponentNext, TransformComponentOld>();
//
//    ASSERT_EQ(3.0f, GetComponent<TransformComponentPrevious>(0)->position.x);
//    ASSERT_EQ(1.0f, GetComponent<TransformComponentNext>(0)->position.x);
//    ASSERT_EQ(2.0f, GetComponent<TransformComponentOld>(0)->position.x);
//}