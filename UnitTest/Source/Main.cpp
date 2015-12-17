#include <gtest/gtest.h>
#include <iostream>
#include <Doremi/Core/Include/Helper/ProximityChecker.hpp>
#include <DoremiEngine/Graphic/Include/Loaders/ModelLoader.hpp>
#include <Utility/DebugLog/Include/ConsoleManager.hpp>
#include <Utility/DebugLog/Include/VirtualConsole.hpp>

int main(int argc, char** argv)
{
    Doremi::Core::ProximityChecker::GetInstance().CheckProximityToEntity(0, 0);
    DoremiEngine::Graphic::ModelLoader test = DoremiEngine::Graphic::ModelLoader();
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    int a;
    std::cin >> a;
    return 0;
}

TEST(Mathematic, addition)
{
    EXPECT_EQ(5, 2 + 2);
    EXPECT_EQ(10, 5 + 5);
}