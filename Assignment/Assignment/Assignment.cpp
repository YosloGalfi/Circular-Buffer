#include <stdio.h>
#include "Comlib.h"
#include <time.h>

using namespace std;

void gen_random(char* s, const int len);

int main(int arg, char* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    srand(time(NULL));

    // Copy arguments to relevant variables
    DWORD delay = atoi(argv[2]);
    size_t bufferSize = atoi(argv[3]) * (long long)1024;
    size_t numberOfMessages = atoi(argv[4]);
    size_t msgSize = 0;

    size_t chunkSize = 256;
    size_t maxSize = bufferSize / 4;

    bool random = false;

    char* message = new char[maxSize];

    // If random is passed in the arguments
    if (strcmp("random", argv[5]) == 0)
    {
        random = true;
    }
    else // Otherwise the msg size will be fixed
    {
        random = false;
        msgSize = atoi(argv[5]);
    }

    // If producer is passed
    if (strcmp("producer", argv[1]) == 0)
    {
        int counter = 1;

        Comlib producer = Comlib(L"buffer", bufferSize, PRODUCER, chunkSize);

        // while not done
        while (counter <= numberOfMessages)
        {
            Sleep(delay);

            size_t randomSize = msgSize;

            // Generate random size 
            if (random)
            {
                randomSize = rand() % maxSize + 2;
            }

            // Generate random message
            gen_random((char*)message, randomSize);

            // Try sending a message until success
            while (true)
            {
                if (producer.Send(message, randomSize, chunkSize))
                {
                    cout << counter << " " << message << endl;
                    counter++;
                    break;
                }
                else
                {
                    Sleep(1);
                }
            }
        }

        Sleep(1000);
        cout << "Done" << endl;
    }
    // If consumer is passed
    else if (strcmp("consumer", argv[1]) == 0)
    {
        int counter = 1;
        size_t size = 0;

        Comlib consumer = Comlib(L"buffer", bufferSize, CONSUMER, chunkSize);

        // While not done
        while (counter <= numberOfMessages)
        {
            Sleep(delay);

            memset(message, '\0', maxSize);

            if (consumer.Recieve(message, size))
            {
                cout << counter << " " << message << endl;
                counter++;
            }
            else
            {
                Sleep(1);
            }
        }

        Sleep(1000);
    }

    return 0;
}

void gen_random(char* s, const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (auto i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
}
