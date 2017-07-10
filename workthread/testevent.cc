/* 
unit test code for event.h 
g++ -o te event.h testevent.cc -std=c++11 -Wall -pthread
*/

#include <iostream>
#include <thread>
#include <unistd.h>

#include <string.h>
//#include "handler.h"
#include "../autot/auto_tchar.h"
#include "event.h"
#include "queue.h"

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;

class handler
{
public:
	handler() = default;
	virtual ~handler() {};
	
	void put(const tstring &t)
	{
		_mq.push(t);
		e_put_data_in_mq.notify_one();
	};
	
	void wait4processover() 
	{
		while (!_mq.empty())
			e_mq_is_empty.timed_wait(1);
	};

	int buffersize() { return (int)_mq.size(); };

	void clearbuffer() { _mq.clear(); };

	bool threadstart()
	{
		if (_workthread != 0)
			return false;

		_exit = false;
		thr = std::thread(&handler::handler_thread, this);
		_workthread = thr.native_handle();
		
		return true;
	}
	
	void threadexit()
	{
		if (_workthread == 0)
			return;

		_exit = true;
		e_put_data_in_mq.notify_one();
		//e_thread_exit.wait();
		if (thr.joinable())
			thr.join();
		
		_workthread = 0;
	}

protected:
	//virtual void handler_func(const T&) = 0;

	virtual void handler_thread()
	{
		while (_exit == false)
		{
			std::cout << "before  e_put_data_in_mq.wait()" << std::endl;

			e_put_data_in_mq.wait();

			while (!_mq.empty())
			{
				tstring t;
				if (_mq.get(t))
				{
					std::cout << t << std::endl;
				}
			}
			
			e_mq_is_empty.notify_one();
		}
		
		//e_thread_exit.notify_one();
		std::cout << "thread exit." << std::endl;
	};

	std::thread thr;
	std::thread::native_handle_type _workthread = 0;
	bool _exit = true;
	queue<tstring> _mq; // message queue.
	
	event e_put_data_in_mq;
	event e_mq_is_empty;
	//event e_thread_exit;
};


int main(void)
{
	handler h;

	h.threadstart();
	TCHAR buff[4];
	int i = 1;
	for (; i <= 8; ++i)
	{
		memset(buff, 0x00, sizeof buff);
		//itoa(i, buff, 10);
		_stprintf(buff, _T("wwtest %d"), i);
		h.put(buff);
		sleep(i);
	}

	h.threadexit();

	std::cout << "main() exit." << std::endl;
}

