// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"

#include <iostream>
#include <assert.h>
#include <sys/param.h>

#include "../src/passphrase_cache.hh"
#include "../src/status_to_string.hh"
#include "../src/Adapter.hh"

using namespace pEp;

extern "C" {
::PEP_STATUS api_test1(::PEP_SESSION session, const char *str, char *bytes, int n, ::stringlist_t *sl)
{
    std::cout << "called api_test1\n";
    return PEP_WRONG_PASSPHRASE;
}

::PEP_STATUS api_test2(::PEP_SESSION session, int n, const char *str, char *bytes, ::stringlist_t *sl)
{
    std::cout << "called api_test2\n";
    return PEP_STATUS_OK;
}
};

int main(int argc, char **argv)
{
    Test::setup(argc, argv);
    Adapter::session.initialize();
    const char *str = "23";
    char *bytes = NULL;
    int n = 42;
    ::stringlist_t *sl = NULL;

    PassphraseCache cache{ 2, std::chrono::seconds(1) };
    cache.add("say");
    cache.add("hello");
    cache.add("world");

    std::cout << "expected: two passphrases\n";
    cache.for_each_passphrase([&](std::string passphrase) {
        std::cout << "'" << passphrase << "'\n";
        return false;
    });

    std::cout << "expected: one passphrase\n";
    cache.for_each_passphrase([&](std::string passphrase) {
        std::cout << "'" << passphrase << "'\n";
        return passphrase != "";
    });

    std::cout << "expected: two passphrases but reverse order\n";
    cache.for_each_passphrase([&](std::string passphrase) {
        std::cout << "'" << passphrase << "'\n";
        return false;
    });

    ::PEP_STATUS status = cache.api(api_test1, Adapter::session(), "23", bytes, n, (::stringlist_t *)NULL);
    assert(status == PEP_WRONG_PASSPHRASE);
    status = cache.api(api_test2, Adapter::session(), n, str, bytes, sl);
    assert(status == PEP_STATUS_OK);

    cache.add("hello");
    cache.add("world");
    std::cout << "expected: two passphrases in reverse order\n";
    PassphraseCache _cache = cache;
    try {
        while (1) {
            std::cout << "'" << _cache.latest_passphrase(_cache) << "'\n";
        }
    } catch (std::underflow_error &) {
    }

    passphrase_cache.add("hello");
    passphrase_cache.add("world");
    std::cout << "two times PEP_STATUS_OK (0), one time PEP_WRONG_PASSPHRASE (2561)\n";
    do {
        status = PassphraseCache::config_next_passphrase();
        std::cout << status_to_string(status) << " (" << status << ")\n";
    } while (status == PEP_STATUS_OK);

    sleep(2);

    std::cout << "expected: no passphrase\n";
    cache.for_each_passphrase([&](std::string passphrase) {
        std::cout << "'" << passphrase << "'\n";
        return false;
    });

    status = cache.api(api_test1, Adapter::session(), str, bytes, n, sl);
    assert(status == PEP_WRONG_PASSPHRASE);
    status = cache.api(api_test2, Adapter::session(), 23, str, bytes, sl);
    assert(status == PEP_STATUS_OK);

    Adapter::session.release();
    return 0;
}
