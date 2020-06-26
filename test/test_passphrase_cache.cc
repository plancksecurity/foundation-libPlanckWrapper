#include <iostream>
#include <unistd.h>
#include <assert.h>

#include "passphrase_cache.hh"

int main()
{
    pEp::PassphraseCache cache{std::chrono::seconds(1)};
    cache.add("hello");
    cache.add("world");

    std::cout << "expected: two passphrases\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << passphrase << "\n"; return false;});

    std::cout << "expected: one passphrase\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << passphrase << "\n"; return true;});

    sleep(2);

    std::cout << "expected: no passphrase\n";
    cache.for_each_passphrase([&](std::string passphrase){std::cout << passphrase << "\n"; return false;});

    return 0;
}

