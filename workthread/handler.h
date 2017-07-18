#ifndef __HANDLER__
#define __HANDLER__

#include <thread>

#include "event.h"
#include "queue.h"

template <class T> class handler
{
public:
	handler() = default;
	virtual ~handler() {};
	
	void put(const T &t) { _mq.push(t); };

	void notify() { e_put_data_in_mq.notify_one(); };
	
	void put_notify(const T &t)
	{
		_mq.push(t);
		e_put_data_in_mq.notify_one();
	};

	void wait4processover() 
	{
		while (!_mq.empty())
		{
			e_put_data_in_mq.notify_one();
			e_after_loop.timed_wait(0);
		}
	};

	int buffersize() { return (int)_mq.size(); };

	void clearbuffer() { _mq.clear(); };

	void forcewait(bool wait_or_not = true) { _force_wait = wait_or_not; }
	bool threadstart()
	{
		if (_th != 0)
			return false;

		_exit = false;
		_thr = std::thread(&handler::handler_thread, this);
		_th = _thr.native_handle();
		
		return true;
	}
	
	std::thread::id gettid() const { return _thr.get_id(); };	
	
	void threadexit()
	{
		if (_th == 0)
			return;

		_exit = true;
		e_put_data_in_mq.notify_one();
		if (_thr.joinable())
			_thr.join();
		
		_th = 0;
	}

protected:
	virtual void handler_func(const T&) = 0;

	virtual void handler_thread()
	{
		while (_exit == false)
		{
			e_put_data_in_mq.wait();

			while (!_force_wait && !_mq.empty())
			{
				T t;
				if (_mq.get(t))
				{
					handler_func(t);
				}
			}
			
			e_after_loop.notify_one();
		}
	};

	std::thread _thr;
	std::thread::native_handle_type _th = 0;
	std::thread::id _tid;
	bool _exit = true;
	bool _force_wait = false;
	queue<T> _mq; // message queue.
	
	event e_put_data_in_mq;
	event e_after_loop;
	
	bool _enable_notify = true;
};

#endif // __HANDLER__
