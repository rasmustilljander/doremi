#include <LoggerProcess.hpp>
#include <iostream> // cin
#include <exception> // std::set_unexpected

int main(int argc, char** argv)
{
    using namespace std;
    LoggerProcess application;
    try
    {
        application.Initialize();
        application.Run();
    }
    catch(std::exception& e)
    {
        cout << "Unknown error: " << e.what() << endl;

#ifdef _DEBUG
        std::cin.get();
#endif
    }
    catch(...)
    {
        cout << "Unknown error." << endl;

        // TODORT debug info
    }

    return 0;
}