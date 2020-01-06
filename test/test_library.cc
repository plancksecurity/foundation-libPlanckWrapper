// small unittest program for libpEpAdapter
// TODO: use Gtest or the like for more modular unit tests!

#include "pc_container.hh"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>


using namespace pEp;


// Producer's data:
class P
{
public:
	P(int i)
	: data{new char[64]}
	{
		snprintf(data, 63, "%i", i);
	}
	
	P(const P&) = delete;
	void operator=(const P&) = delete;
	
	~P()
	{
		std::cerr << "~P(): data=" << (data ? data : "(NULL)" ) << '\n';
		delete[] data;
	}
	
	char* data;
};


// Consumer's data:
class C
{
public:
	C(int _i) : i(_i) {}
	int i;
};

pc_container<P,C> pc;

void consumer_thread()
{
	bool keep_running = true;
	int sum = 0;
	while(keep_running)
	{
		for(auto& q : pc)
		{
		    switch(q.state())
		    {
			case PC_State::Created:
			{
				const int value = atoi( q.pdata->data );
				if(value<0)
				{
					std::cerr << "Q\n";
					keep_running = false;
				}else{
					std::cerr << "C";
					q.cdata = new C( value );
					sum += q.cdata->i;
				}
				break;
			}
			case PC_State::Deleted:
			{
				std::cerr << "D";
				sum -= q.cdata->i;
				delete q.cdata;
				break;
			}
			case PC_State::Changed:
			{
				std::cerr << "X";
				sum -= q.cdata->i;
				delete q.cdata;
				q.cdata = new C( atoi( q.pdata->data ) );
				sum += q.cdata->i;
				break;
			}
			default: throw "Illegal state";
			}
		}
	}
	
	std::cout << "Consumer sum: " << sum << ".\n";
}

int main()
{
	for(int i=0; i<10; ++i)
	{
		pc.insert( new P(i) );
	}
	
	std::thread t{ &consumer_thread };
	
	for(int i=10; i<100; ++i)
	{
		pc.insert( new P(i) );
	}
	
	while( !pc.empty() )
	{
		auto q = pc.begin();
		delete q->pdata;
		pc.erase( q );
	}
	
	pc.insert( new P(-1) );
	
	t.join();
}
