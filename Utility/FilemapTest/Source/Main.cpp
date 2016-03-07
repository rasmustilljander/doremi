#include <cstdio>
#include <iostream>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/ArbitrarySizeCirclebuffer.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/ArbitraryStaticData.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMapMutex.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>

using namespace std;
using namespace Doremi::Utilities::IO;
using namespace Doremi::Utilities::Memory;
using namespace Doremi::Utilities::Chrono;

void gen_random(char* s, const int len)
{
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";

    for(auto i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
}

int main(int argc, char** argv)
{
    if(argc != 6)
    {
        printf("Incorrect useage1");
        return 1;
    }

    const int clientDelay = atoi(argv[2]);
    const int memSize = atoi(argv[3]);
    printf("memSize %d", memSize);
    const int numMsg = atoi(argv[4]);
    const char* msgSize = argv[5];
    int msgSizeInt = 0;

    if(msgSize == "random")
    {
        msgSizeInt = rand() % 10000;
    }
    else
    {
        msgSizeInt = atoi(argv[5]);
    }
    FileMap filemap;
    filemap.Initialize("__MY_FILE_MAP__", memSize);

    FileMapMutex mutex;
    mutex.Initialize("__MY_MUTEX__");

    ArbitrarySizeCirclebuffer circlebuffer;
    try
    {
        circlebuffer.Initialize(memSize + sizeof(ArbitraryStaticData), &mutex);
    }
    catch(const std::exception& e)
    {
        printf("error: %s", e.what());
    }

    Timer timer;
    double time = 0;
    printf("argv[1]: %s", argv[1]);
    if(argv[1] == "producer")
    {
        int producedMessages = 0;
        while(producedMessages < numMsg)
        {
            timer.Reset();
            time = 0;
            while(time < clientDelay)
            {
                time += timer.GetElapsedTimeInSeconds();
            }

            CircleBufferHeader header;
            header.packageSize = msgSizeInt;
            header.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
            char* msg = new char(msgSizeInt);
            gen_random(msg, msgSizeInt);
            while(true)
            {
                try
                {
                    circlebuffer.Produce(header, msg);
                    break;
                }
                catch(...)
                {
                }
            }
            ++producedMessages;
            printf("%d %s", producedMessages, msg);
            delete msg;
        }
    }
    else if(argv[1] == "consumer")
    {
        int consumedMessaged = 0;
        while(consumedMessaged < numMsg)
        {
            timer.Reset();
            time = 0;
            while(time < clientDelay)
            {
                time += timer.GetElapsedTimeInSeconds();
            }
            char* msg = new char(10000);
            CircleBufferHeader* header = new CircleBufferHeader();

            bool success = false;
            while(success != true)
            {
                success = circlebuffer.Consume(header, msg, 10000);
            }
            ++consumedMessaged;
            printf("%d %s", consumedMessaged, msg);
            delete msg;
        }
    }
    else
    {
        printf("Incorrect useage2");
        return 1;
    }
    return 0;
}