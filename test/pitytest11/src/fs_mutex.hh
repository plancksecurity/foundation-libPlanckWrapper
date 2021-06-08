#ifndef FS_MUTEX
#define FS_MUTEX

#include <iostream>

namespace pEp {
    namespace PityTest11 {
        // a very primitive IPC sync method
        // also unreliable
        // but good enough for what i just needed it for
        class fs_mutex {
        public:
            fs_mutex() = delete;
            fs_mutex(std::string mutexpath);

            void aquire() const;
            void release() const;

        private:
            const std::string mutexpath;
        };
    } // namespace PityTest11
} // namespace pEp

#endif // FS_MUTEX