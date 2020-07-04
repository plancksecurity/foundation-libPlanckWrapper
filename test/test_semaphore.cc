#include <iostream>
#include <thread>
#include <unistd.h>
#include "Semaphore.hh"

using namespace std;
using namespace pEp;

int main()
{
    Semaphore semaphore;

    thread thread1([&](){
            cout << "1: before stop\n";
            semaphore.stop();
            cout << "1: now on stop\n";
            semaphore.try_wait();
            cout << "1: and on go again\n";
            semaphore.try_wait();
            cout << "1: keep going\n";
        });

    sleep(1);

    thread thread2([&](){
            cout << "2: setting go\n";
            semaphore.go();
        });

    thread1.join();
    thread2.join();

    return 0;
}

