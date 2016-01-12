// Project specific
#include <Game.hpp>

// Third party

// Standard libraries
#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#else
#error Platform not supported
#endif
#include <exception>
#include <iostream>
#include <Utility/MemoryManager/Include/Allocator/Pool/FixedSizePoolAllocator.hpp>
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>
#include <vector>

struct SomeThing
{
    SomeThing()
    {
        a = 3;
        b = a * a;
        c = b * b;
    }
    float a;
    float b;
    float c;
};

#ifdef _WIN32
int main(int argc, const char* argv[])
#else
#error Platform not supported
#endif
{
    // TODORT
    // This row is required later as it disables the standard output terminal, we do now want that.
    // However, as people are currently using cout for debugging we'll need this for the moment.
    // FreeConsole();

    using namespace Utility::MemoryManager;
    using namespace Utility::Timer;
    FixedSizePoolAllocator<SomeThing> a = FixedSizePoolAllocator<SomeThing>();
    a.Initialize(100000, 0);
    int i = 0;


    // Crude test, do not push on git
    MeasureTimer& mt = MeasureTimer::GetInstance();
    mt.Reset("ca").Start("ca");
    using namespace std;
    vector<SomeThing*> b;
    vector<SomeThing*> c;

    for(size_t i = 0; i < 100000; ++i)
    {
        b.push_back(a.Allocate());
    }
    a.Clear();
    b.clear();
    float caf = mt.Stop("ca").GetSeconds("ca");

    // Crude test, do not push on git

    mt.Reset("ra").Start("ra");
    for(size_t i = 0; i < 100000; ++i)
    {
        c.push_back(new SomeThing());
    }
    for(auto item : c)
        delete item;
    c.clear();
    float raf = mt.Stop("ra").GetSeconds("ra");
    // Crude test, do not push on git


    try
    {
        Doremi::GameMain gameMain = Doremi::GameMain();
        gameMain.Start();
    }
    catch(const std::exception& e)
    {
        std::cout << "Unhandled exception: " << e.what() << std::endl;
        // TODORT log
    }
    catch(...)
    {
        std::cout << "Unhandled unknown exception" << std::endl;
        // TODORT log
    }

#ifdef _DEBUG
    system("pause");
#endif
    return 0;
}