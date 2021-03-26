// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_CALL_WITH_LOCK_HH
#define LIBPEPADAPTER_CALL_WITH_LOCK_HH

#include <mutex>

namespace pEp {
    extern std::mutex call_with_lock_mutex;

    // TODO: use && and std::forward<> to avoid copying of the arguments.
    // It is not relevant, yet, because at the moment we use this function
    // template only for init() and release() which have cheap-to-copy pointer
    // parameters only
    template<class R, class... Args>
    R call_with_lock(R (*fn)(Args...), Args... args)
    {
        std::lock_guard<std::mutex> L(call_with_lock_mutex);
        return fn(args...);
    }


} // namespace pEp

#endif // LIBPEPADAPTER_CALL_WITH_LOCK_HH
