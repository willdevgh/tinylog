#ifndef __HANDLER__
#define __HANDLER__

#include "event.h"
#include "queue.h"

template <class T> class handler
{
public:
	handler() = default;
	virtual ~handler() {};
	
	void put(const T &t)
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
	virtual void handler_func(const T&) = 0;

	virtual void handler_thread()
	{
		while (_exit == false)
		{
			e_put_data_in_mq.wait();

			while (!_mq.empty())
			{
				T t;
				if (_mq.get(t))
				{
					handler_func(t);
				}
			}
			
			e_mq_is_empty.notify_one();
		}
		
		//e_thread_exit.notify_one();
	};

	std::thread thr;
	std::thread::native_handle_type _workthread = 0;
	bool _exit = true;
	queue<T> _mq; // message queue.
	
	event e_put_data_in_mq;
	event e_mq_is_empty;
	//event e_thread_exit;
};

#endif // __HANDLER__
