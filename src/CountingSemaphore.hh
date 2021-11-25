// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_COUNTING_SEMAPHORE_HH
#define LIBPEPADAPTER_COUNTING_SEMAPHORE_HH

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace pEp {
    class CountingSemaphore {
        std::mutex mtx;
        std::condition_variable cv;

        // To synchronize threads, ALWAYS use <atomic> types
        std::atomic_uint _count;

    public:
        CountingSemaphore(unsigned count = 0) : _count(count) {}

        // Change the stored count.
        CountingSemaphore& operator=(unsigned count)
        {
            std::unique_lock<std::mutex> lock(mtx);
            _count.store (count);
            if (count != 0)
                cv.notify_one ();
            return *this;
        }

        // Return the current stored count.
        unsigned load()
        {
            return _count.load ();
        }

        // Atomically decrement the stored count, blocking in case the count
        // is currently zero.  When the method terminates the counter is
        // guaranteed to have been decremented by one unit.
        //
        // This is Dijkstra's P operation, used to atomically acquire a resource.
        void p()
        {
            std::unique_lock<std::mutex> lock(mtx);
            // FIXME: is the loop even needed?  Any received notification will
            // wake up ony one thread, which will see the count as non-zero...
            while (_count.load() == 0)
                cv.wait(lock);
            _count --;
        }

        // Atomically increment the stored count.  This may wake up one thread
        // which is currently executing the p method.
        //
        // This is Dijkstra's V operation, used to atomically release a resource.
        void v()
        {
            std::unique_lock<std::mutex> lock(mtx);
            _count ++;
            cv.notify_one();
        }
    };
} // namespace pEp

#endif // LIBPEPADAPTER_COUNTING_SEMAPHORE_HH
