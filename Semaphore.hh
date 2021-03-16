// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_SEMAPHORE_HH
#define LIBPEPADAPTER_SEMAPHORE_HH

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace pEp {
    class Semaphore {
        std::mutex mtx;
        std::condition_variable cv;
        // Atomic types are types that encapsulate a value whose access is guaranteed
        // to not cause data races and can be used to synchronize memory accesses among
        // different threads.
        // To synchronize threads, ALWAYS use <atomic> types
        std::atomic_bool _stop;

    public:
        Semaphore() : _stop(false) { }

        void stop()
        {
            std::unique_lock<std::mutex> lock(mtx);
            _stop.store(true);
        }

        void try_wait()
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (!_stop.load()) {
                return;
            }

            while(_stop.load()) {
                cv.wait(lock);
            }
        }

        void go()
        {
            std::unique_lock<std::mutex> lock(mtx);
            _stop.store(false);
            cv.notify_all();
        }
    };
}

#endif // LIBPEPADAPTER_SEMAPHORE_HH


