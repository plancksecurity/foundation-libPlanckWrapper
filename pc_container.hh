#ifndef PEP_LIB_PC_CONTAINER
#define PEP_LIB_PC_CONTAINER

// Conainer adapter that contains a container and a producer/consume queue
// that holds references to all changed elements


#include "locked_queue.hh"

namespace pEp
{

enum class PC_State { Illegal = 0, Created = 1, Deleted = 2, Changed = 3 };


// Producer/Consumer container.
// 
// The "Producer" works on a std::list: inserts, changes, erases elements and 
// informs a "Consumer" via a queue about the changes.
//
// The "Consumer" can poll for changes and process them asynchronously.
//
// Pdata = data of the producer. May be read by the consumer.
// Cdata = data of the consumer.
template<class Pdata, class Cdata>
class pc_container
{
public:
    struct PC
    {
        Pdata* pdata; // data of the producer. Will be nullptr for erased elements
        Cdata* cdata; // data of the consumer.
        
        PC_State state() const noexcept { return PC_State((pdata!=nullptr) + (cdata!=nullptr)*2); }
    };
    
    typedef std::list<PC> Container;
    
    typename Container::const_iterator begin() const noexcept { return c.cbegin(); }
    typename Container::const_iterator end()   const noexcept { return c.cend();   }
    typename Container::iterator       begin() noexcept { return c.begin(); }
    typename Container::iterator       end()   noexcept { return c.end();   }
    
    std::size_t size()  const noexcept { return c.size();  }
    bool        empty() const noexcept { return c.empty(); }


    void insert(Pdata* pd)
    {
        c.push_back({ pd, nullptr });
    }
    
    // Beware: does not delete *pdata nor *cdata! That's producer's and consumer's task!
    void erase(typename Container::const_iterator pos)
    {
        c.erase(pos);
    }
    
    // clear the container. Delete all *pdata via custom deleter functor.
    void clear(std::function<void(Pdata*)> deleter = [](Pdata *e) { delete e; })
    {
        for(auto q=begin(); q!=end(); ++q)
        {
            deleter(q->pdata);
            q->pdata = nullptr;
            erase(q);
        }
    }

private:
    Container c;
};

} // end of namespace pEp

#endif // PEP_LIB_PC_CONTAINER
