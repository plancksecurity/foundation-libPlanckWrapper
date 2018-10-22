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
    std::size_t size()  const noexcept { return c.size();  }
    bool        empty() const noexcept { return c.empty(); }


    //////////////////////////////////
    // Producer's API:
    //////////////////////////////////
    
    void insert(Pdata* pd)
    {
        c.push_back({ pd, nullptr });
        changed.push_back( c.back() );
    }
    
    // Beware: does not delete *pdata nor *cdata! That's producer's and consumer's task!
    void erase(typename Container::const_iterator pos)
    {
        changed.push_back( *pos );
        c.erase(pos);
    }
    
    // notify Consumer about the changed element
    void change(typename Container::const_iterator pos)
    {
        changed.push_back( *pos );
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
    
    //////////////////////////////////
    // Consumer's API:
    //////////////////////////////////

    std::size_t changed_elements() const { return changed.size(); }
    bool        has_changed()      const { return !changed.empty(); }
    
    // got oldest element in change queue.
    //
    // pc.pdata != nullptr && pc.cdata == nullptr : freshly inserted element
    // pc.pdata != nullptr && pc.cdata != nullptr : changed element
    // pc.pdata == nullptr && pc.cdata != nullptr : erased element
    //
    // Beware: this blocks if changed queue is empty!
    PC get_changed()
    {
        PC pc = changed.pop_front();
        
        // does the element still exists?
        if( pc.pdata && 
            std::find_if( c.cbegin(), c.cend(), [&pc](const PC& element) { return pc.pdata == element.pdata; } ) == c.cend()
          )
        {
            // No, not anymore. So mark it as erased element for the consumer:
            pc.pdata = nullptr;
        }
        
        return pc;
    }

private:
    Container c;
    ::utility::locked_queue< PC > changed;
};

} // end of namespace pEp

#endif // PEP_LIB_PC_CONTAINER
