#include <LoggerProcess.hpp>
#include <iostream> // cin
#include <exception> // std::set_unexpected

int main(int argc, char** argv)
{
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
        // TODORT debug info
    }

    return 0;
}