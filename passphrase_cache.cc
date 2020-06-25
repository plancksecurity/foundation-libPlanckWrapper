#include "passphrase_cache.hh"

namespace pEp {
    void PassphraseCache::add(std::string passphrase)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        cleanup();
        _cache.push_back({passphrase, clock::now()});
    }

    bool PassphraseCache::for_each_passphrase(passphrase_callee& callee)
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
        for (auto entry = _cache.begin(); entry != _cache.end(); ) {
            if (entry->tp < clock::now() - _timeout)
                _cache.erase(entry);
            else
                break;
        }
    }
};

