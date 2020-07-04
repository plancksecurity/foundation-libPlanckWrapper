#include <mutex>
#include <condition_variable>

namespace pEp {
    class Semaphore {
    public:
        Semaphore() : _stop(false) {}

        void stop()
        {
            std::unique_lock<std::mutex> lock(mtx);
            _stop = true;
        }

        void try_wait()
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (!_stop)
                return;

            while(_stop){
                cv.wait(lock);
            }
        }

        void go()
        {
            std::unique_lock<std::mutex> lock(mtx);
            _stop = false;
            cv.notify_all();
        }

    private:
        std::mutex mtx;
        std::condition_variable cv;
        bool _stop;
    };
}

