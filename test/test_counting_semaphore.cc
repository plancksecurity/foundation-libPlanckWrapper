// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include <iostream>
#include <thread>
#include <cassert>
#include "../src/std_utils.hh"
#include "../src/CountingSemaphore.hh"

using namespace std;
using namespace pEp;

thread make_two_threads_for_many_threads()
{
    thread res([&]() {
        CountingSemaphore semaphore;
        semaphore = 3;

        thread thread1([&]() {
            semaphore.p ();
            semaphore.p ();
            semaphore.p ();
            // Now the count is at 0, assuming the other thread has not
            // started yet; otherwise it should be at 1.

            semaphore.p ();
            //cout << "1: done\n";
        });

        Utils::sleep_millis(1000);

        thread thread2([&]() {
            semaphore.v();
        });

        thread1.join();
        thread2.join();

        assert (semaphore.load() == 0);
    });
    return res;
}

void many_threads()
{
    int i;

#define N 1000
    thread threads[N];
    std::cout << "starting " << N
              << " threads, each starting and joining a thread pair...\n";
    for (i = 0; i < N; i ++)
        threads [i] = make_two_threads_for_many_threads ();
    std::cout << "joining threads...\n";
    for (i = 0; i < N; i ++)
        threads [i].join();
#undef N
    std::cout << "many_threads done.\n\n";
}

void v_times (CountingSemaphore &semaphore,
              int times)
{
    std::cout << "V'ing " << times << " times...\n";
    for (int i = 0; i < times; i ++)
        semaphore.v ();
    std::cout << "... I V'd " << times << " times.\n";
}

void p_times (CountingSemaphore &semaphore,
              int times)
{
    std::cout << "P'ing " << times << " times...\n";
    for (int i = 0; i < times; i ++)
        semaphore.p ();
    std::cout << "... I P'd " << times << " times.\n";
}

void few_threads()
{
#define HALF_N (1000 * 1000 * 10)
#define N (HALF_N * 2)
    CountingSemaphore semaphore;

    thread thread_p1([&]() {
        p_times (semaphore, HALF_N);
    });
    thread thread_v([&]() {
        v_times (semaphore, N);
    });
    thread thread_p2([&]() {
        p_times (semaphore, HALF_N);
    });
#undef HALF_N
#undef N
    thread_v.join();
    thread_p1.join();
    thread_p2.join();

    assert (semaphore.load() == 0);

    std::cout << "few_thrads done.\n\n";
}

int main()
{
    many_threads ();
    few_threads ();
    return 0;
}