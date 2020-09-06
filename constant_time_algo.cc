#include "constant_time_algo.hh"

namespace pEp
{
    bool constant_time_equal(const std::string& a, const std::string& b)
    {
        if(a.size() != b.size())
            return false;
        
        unsigned d = 0;
        for(std::size_t idx = 0; idx<a.size(); ++idx)
        {
            d |= ( static_cast<unsigned>(a[idx]) ^ static_cast<unsigned>(b[idx]) );
        }
        
        // if d is still 0, the strings are equal.
        return d == 0;
    }

} // end of namespace pEp
