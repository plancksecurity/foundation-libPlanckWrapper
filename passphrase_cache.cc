#include "passphrase_cache.hh"

namespace pEp {
    void PassphraseCache::add(std::string passphrase)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        while (_cache.size() >= _max_size)
            _cache.pop_front();
        _cache.push_back({passphrase, clock::now()});
    }

    bool PassphraseCache::for_each_passphrase(const passphrase_callee& callee)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        cleanup();

        for (auto entry=_cache.begin(); entry!=_cache.end(); ++entry) {
            if (callee(entry->passphrase)) {
                refresh(entry);
                return true;
            }
        }

        return false;
    }

    void PassphraseCache::cleanup()
    {
        while (!_cache.empty() && _cache.front().tp < clock::now() - _timeout)
            _cache.pop_front();
    }

    void PassphraseCache::refresh(cache::iterator entry)
    {
        entry->tp = clock::now();
        _cache.splice(_cache.end(), _cache, entry);
    }
};

