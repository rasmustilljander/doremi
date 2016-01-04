#include <gtest/gtest.h>
#include <iostream>
#include <Doremi/Core/Include/Helper/ProximityChecker.hpp>
#include <DoremiEngine/Graphic/Include/Loaders/ModelLoader.hpp>
#include <Utility/DebugLog/Include/ConsoleManager.hpp>
#include <Utility/DebugLog/Include/VirtualConsole.hpp>

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    int a;
    std::cin >> a;
    return 0;
}
