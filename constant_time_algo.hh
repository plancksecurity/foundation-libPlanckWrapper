#pragma once

#include <string>

namespace pEp
{
    // Returns false if a.size() != b.size().
    // Compares always _all_ characters of 'a' and 'b' so runtime does not
    // depends on the character position where the strings differ.
    // Use this function instead of operator== if timing sidechannel attack
    // might be a security problem.
    bool constant_time_equal(const std::string& a, const std::string& b);

} // end of namespace pEp
