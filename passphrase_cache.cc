#include <cassert>
#include "passphrase_cache.hh"

namespace pEp {
    PassphraseCache::cache_entry::cache_entry(std::string p, time_point t) :
            passphrase{p, 0, PassphraseCache::cache_entry::max_len}, tp{t}
    { }

    PassphraseCache::PassphraseCache(int max_size, duration timeout) :
            _max_size{max_size}, _timeout{timeout}, _which{_cache.end()}
    { }

    PassphraseCache::PassphraseCache(const PassphraseCache& second) :
            _cache{second._cache}, _max_size{second._max_size},
            _timeout{second._timeout}, _which{_cache.end()}
    {
        cleanup();
    }

    PassphraseCache& PassphraseCache::operator=(const PassphraseCache& second)
    {
        _cache = second._cache;
        _max_size = second._max_size;
        _timeout = second._timeout;
        _which = _cache.end();
        cleanup();
        return *this;
    }

    const char *PassphraseCache::add(std::string passphrase)
    {
        assert(_which == _cache.end()); // never modify while iterating
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
        assert(_which == _cache.end()); // never modify while iterating
        while (!_cache.empty() && _cache.front().tp < clock::now() - _timeout)
            _cache.pop_front();
    }

    void PassphraseCache::refresh(cache::iterator entry)
    {
        assert(_which == _cache.end()); // never modify while iterating
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

	PEP_STATUS PassphraseCache::messageToSend(PassphraseCache& cache, PEP_SESSION session)
	{
        static pEp::PassphraseCache _copy;
        static bool new_copy = true;
        if (new_copy) {
            _copy = cache;
            new_copy = false;
        }
        try {
            ::config_passphrase(session, _copy.latest_passphrase());
            return PEP_STATUS_OK;
        }
        catch (pEp::PassphraseCache::Empty&) {
            new_copy = true;
            return PEP_PASSPHRASE_REQUIRED;
        }
        catch (pEp::PassphraseCache::Exhausted&) {
            new_copy = true;
            return PEP_WRONG_PASSPHRASE;
        }
	}
};

