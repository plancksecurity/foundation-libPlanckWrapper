#include <iostream>
#include <exception>
#include <unistd.h>
#include <assert.h>

#include "passphrase_cache.hh"

extern "C" {
    PEP_STATUS api_test1(PEP_SESSION session, const char *str, char *bytes, int n, ::stringlist_t *sl)
    {
        std::cout << "called api_test1\n";
        return PEP_WRONG_PASSPHRASE;
    }

    PEP_STATUS api_test2(PEP_SESSION session, int n, const char *str, char *bytes, ::stringlist_t *sl)
    {
        std::cout << "called api_test2\n";
        return PEP_STATUS_OK;
    }
};

int main()
{
    PEP_SESSION session;
    PEP_STATUS status = ::init(&session, NULL, NULL);
    assert(status == PEP_STATUS_OK);
    assert(session);

    const char *str = "23";
    char *bytes = NULL;
    int n = 42;
    ::stringlist_t *sl = NULL;

    pEp::PassphraseCache cache{2, std::chrono::seconds(1)};
    cache.add("say");
    cache.add("hello");
    cache.add("world");

    std::cout << "expected: two passphrases\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << "'" << passphrase << "'\n"; return false;});

    std::cout << "expected: one passphrase\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << "'" << passphrase << "'\n"; return passphrase != "";});

    std::cout << "expected: two passphrases but reverse order\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << "'" << passphrase << "'\n"; return false;});

    status = cache.api(api_test1, session, "23", bytes, n, (::stringlist_t *) NULL);
    assert(status == PEP_WRONG_PASSPHRASE);
    status = cache.api(api_test2, session, n, str, bytes, sl);
    assert(status == PEP_STATUS_OK);

    std::cout << "expected: two passphrases in order\n";

    pEp::PassphraseCache _cache = cache;
    try {
        while (1) {
            std::cout << "'" << _cache.latest_passphrase() << "'\n";
        }
    }
    catch (std::underflow_error&) { }

    sleep(2);

    std::cout << "expected: no passphrase\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << "'" << passphrase << "'\n"; return false;});

    status = cache.api(api_test1, session, str, bytes, n, sl);
    assert(status == PEP_WRONG_PASSPHRASE);
    status = cache.api(api_test2, session, 23, str, bytes, sl);
    assert(status == PEP_STATUS_OK);

    ::release(session);
    return 0;
}

