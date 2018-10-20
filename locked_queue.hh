// this file is under GNU GPL 3.0, see LICENSE.txt


#pragma once

#include <list>
#include <condition_variable>
#include <mutex>

namespace utility
{
    template<class T, void(*Deleter)(T) = [](T e) { delete e; } >
    class locked_queue
    {
        typedef std::recursive_mutex     Mutex;
        typedef std::unique_lock<Mutex>  Lock;
        
        Mutex  _mtx;
        std::condition_variable_any  _cv;
        std::list<T> _q;

    public:
        ~locked_queue()
        {
            clear();
        }

        void clear()
        {
            Lock L(_mtx);
            for(auto& element : _q)
            {
                Deleter(element);
            }
            _q.clear();
        }

        // undefined behavior when queue empty
        T& back()
        {
            Lock lg(_mtx);
            return _q.back();
        }

        // undefined behavior when queue empty
        T& front()
        {
            Lock lg(_mtx);
            return _q.front();
        }

        // returns a copy of the last element.
        // blocks when queue is empty
        T pop_back()
        {
            Lock L(_mtx);
            _cv.wait(L, [&]{ return !_q.empty(); } );
            T r{std::move(_q.back())};
            _q.pop_back();
            return r;
        }

        // returns a copy of the first element.
        // blocks when queue is empty
        T pop_front()
        {
            Lock L(_mtx);
            _cv.wait(L, [&]{ return !_q.empty(); } );
            T r{std::move(_q.front())};
            _q.pop_front();
            return r;
        }


        // returns true and set a copy of the last element and pop it from queue if there is any
        // returns false and leaves 'out' untouched if queue is empty even after 'end_time'
        bool try_pop_back(T& out, std::chrono::steady_clock::time_point end_time)
        {
            Lock L(_mtx);
            if(! _cv.wait_until(L, end_time, [this]{ return !_q.empty(); } ) )
            {
                return false;
            }
            
            out = std::move(_q.back());
            _q.pop_back();
            return true;
        }

 
        // returns true and set a copy of the first element and pop it from queue if there is any
        // returns false and leaves 'out' untouched if queue is empty even after 'end_time'
        bool try_pop_front(T& out, std::chrono::steady_clock::time_point end_time)
        {
            Lock L(_mtx);
            if(! _cv.wait_until(L, end_time, [this]{ return !_q.empty(); } ) )
            {
                return false;
            }
            
            out = std::move(_q.front());
            _q.pop_front();
            return true;
        }

        void push_back(const T& data)
        {
            {
                Lock L(_mtx);
                _q.push_back(data);
            }
            _cv.notify_one();
        }


        void push_front(const T& data)
        {
            {
                Lock L(_mtx);
                _q.push_front(data);
            }
            _cv.notify_one();
        }

        size_t size()
        {
            Lock lg(_mtx);
            return _q.size();
        }

        bool empty()
        {
            Lock lg(_mtx);
            return _q.empty();
        }
    };
};
