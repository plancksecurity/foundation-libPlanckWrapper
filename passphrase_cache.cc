#include <exception>
#include "passphrase_cache.hh"

namespace pEp {
    const char *PassphraseCache::add(std::string passphrase)
    {
        std::lock_guard<std::mutex> lock(_mtx);

        if (passphrase != "") {
            while (_cache.size() >= _max_size)
                _cache.pop_front();
            _cache.emplace_back(cache_entry(passphrase, clock::now()));
        }

        return passphrase.c_str();
    }

    bool PassphraseCache::for_each_passphrase(const passphrase_callee& callee)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        cleanup();

        if (callee(""))
            return true;

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

    const char *PassphraseCache::latest_passphrase()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        
        if (_cache.empty()) {
            _which = _cache.end();
            throw Empty();
        }

        if (_which == _cache.begin()) {
            _which = _cache.end();
            throw Exhausted();
        }

        --_which;
        return _which->passphrase.c_str();
    }
};

