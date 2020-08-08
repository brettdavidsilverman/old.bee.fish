#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using namespace std;

using namespace boost::interprocess;

int main(int argc, char *argv[])
{
    cout << "Hello World" << endl;

    size_t size = getpagesize() * 100;
    void *map = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANON,
        -1,
        0);

    if ((long int)map <= 0)
    {
        perror("Couldn't create map.");
        return -1;
    }

    cout << map << endl;

    string line;
    interprocess_mutex mutex;
    memcpy(map, &mutex, sizeof(mutex));

    struct Memory
    {
        interprocess_mutex mutex;
        char data[];
    };

    Memory *memory = (Memory *)map;
    memory->mutex.lock();

    if (fork() == 0)
    {
        cout << map << endl;

        // child process
        memory->mutex.lock();

        cout << "Child" << endl;
        cout << memory->data << endl;

        memory->mutex.unlock();
    }
    else
    {
        cout << "Parent" << endl;

        while (!cin.eof())
        {
            getline(cin, line);
            if (line.length() == 0)
                break;
            strcpy(memory->data, line.c_str());
        }

        memory->mutex.unlock();
    }

    munmap(map, size);

    cout << "Finished" << endl;
}
