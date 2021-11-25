// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_SEMAPHORE_HH
#define LIBPEPADAPTER_SEMAPHORE_HH

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

        CountingSemaphore& operator=(unsigned count)
        {
            std::unique_lock<std::mutex> lock(mtx);
            _count.store (count);
            if (count != 0)
                cv.notify_one ();
            return *this;
        }

        unsigned load()
        {
            return _count.load ();
        }

        void p()
        {
            std::unique_lock<std::mutex> lock(mtx);
            // FIXME: is the loop even needed?  Any received notification will
            // wake up ony one thread, which will see the count as non-zero...
            while (_count.load() == 0) {
                //std::cout << "p: waiting...\n";
                cv.wait(lock);
            }
            _count --;
            //std::cout << "after p: " << _count.load () << "\n";
        }

        void v()
        {
            std::unique_lock<std::mutex> lock(mtx);
            _count ++;
            //std::cout << "after v: " << _count.load () << "\n";
            cv.notify_one();
        }
    };
} // namespace pEp

#endif // LIBPEPADAPTER_SEMAPHORE_HH
