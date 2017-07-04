#ifndef __EVENT__
#define __EVENT__

#include "lock.h"
#include <condition_variable>
#include <chrono>

class event final
{
public:
	event() = default;
	~event() = default;
	event(const event&) = delete;
	event(event&&) = delete;
	event& operator=(event&) = delete;
	event& operator=(event&&) = delete;
	
	void wait()
	{
		scopedlock slock;
		_condition.wait(slock);
	};
	
	void timed_wait(int sec)
	{
		scopedlock slock;
		_condition.wait_for(slock, std::chrono::seconds(sec));
	}
	
	void notify_one() 
	{
		_condition.notify_one();
	}

	void notify_all()
	{
		_condition.notify_all();
	}

private:
	std::condition_variable_any _condition;
};


#endif // __EVENT__
