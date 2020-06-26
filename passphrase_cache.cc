#include "passphrase_cache.hh"

namespace pEp {
    void PassphraseCache::add(std::string passphrase)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _cache.push_back({passphrase, clock::now()});
    }

    bool PassphraseCache::for_each_passphrase(const passphrase_callee& callee)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        cleanup();

        for (auto entry : _cache) {
            if (callee(entry.passphrase))
                return true;
        }

        return false;
    }

    void PassphraseCache::cleanup()
    {
        while (!_cache.empty() && _cache.front().tp < clock::now() - _timeout)
            _cache.pop_front();
        while (_cache.size() > _max_size)
            _cache.pop_front();
    }
};

