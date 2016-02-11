#include <LoggerProcess.hpp>
#include <iostream> // cin
#include <exception> // std::set_unexpected

void PrintArguments(int argc, char** argv)
{
    using namespace std;
    cout << "Printing arguments" << endl;

    const int limit = argc;
    for(int i = 0; i < limit; ++i)
    {
        cout << "Argument[" << i << "] = " << argv[i] << endl;
    }
}

void PrintUseage()
{
    using namespace std;
    std::cout << "Logger process is now online." << endl;
    std::cout << "If this crashes (or is shutdown manually) and the game is still running the game will be blocked." << endl;
    std::cout << "This process can safely be shutdown manually when the game has ended." << endl;
    std::cout << "This process shuts down automatically after a set timeout after not receiving any messages for a while." << endl;
}

int main(int argc, char** argv)
{
    PrintUseage();
#ifdef _DEBUG
    PrintArguments(argc, argv);
#endif

    int uniqueIdentifier = 0;
    if(argc == 1)
    {
        // Assume default name
        using namespace std;
        cout << "Assumes default name on filemap." << endl;
    }
    else if(argc == 2)
    {
        using namespace std;
        // Assume started from game with unique identifier
        uniqueIdentifier = atoi(argv[1]);

        // Check so that i's not zero
        if(uniqueIdentifier == 0)
        {
            cout << "Invalid unique identifier for filemap." << endl;
            cout << "Assumes default name on filemap." << endl;
        }
    }
    else
    {
        // Something is probably wrong wrong
        using namespace std;
        cout << "A confusing amount of arguments: " << argc << "." << endl;
        cout << "Assumes default name on filemap." << endl;
    }

    LoggerProcess application;
    try
    {
        application.Initialize(uniqueIdentifier);
        application.Run();
    }
    catch(std::exception& e)
    {
        using namespace std;
        cout << "Unknown error: " << e.what() << endl;

#ifdef _DEBUG
        std::cin.get();
#endif
    }
    catch(...)
    {
        using namespace std;
        cout << "Unknown error." << endl;
    }

    return 0;
}