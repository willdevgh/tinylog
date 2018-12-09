#ifndef __HANDLER__
#define __HANDLER__

#include <thread>

#include "event.h"
#include "queue.h"

namespace tl {

template <class T> class handler
{
public:
	handler() = default;
	virtual ~handler() = default;
	
	void put(const T &t) { _que.push(t); };

	void notify() { e_put_data_in_que.notify_one(); };
	
	void put_notify(const T &t)
	{
		_que.push(t);
		e_put_data_in_que.notify_one();
	};

	void wait4processover() 
	{
		while (!_que.empty())
		{
			e_put_data_in_que.notify_one();
			e_after_loop.timed_wait(0);
		}
	};

	int buffersize() { return (int)_que.size(); };

	void clearbuffer() { _que.clear(); };

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
		e_put_data_in_que.notify_one();
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
			e_put_data_in_que.wait();

			while (!_force_wait && !_que.empty())
			{
				T t;
				if (_que.get(t))
				{
					handler_func(t);
				}
			}
			
			e_after_loop.notify_one();
		}
	};

	std::thread _thr;
	std::thread::native_handle_type _th = 0;
	bool _exit = true;
	bool _force_wait = false;
	queue<T> _que; // log line queue.
	event e_put_data_in_que;
	event e_after_loop;
};

} // namespace tl

#endif // __HANDLER__
