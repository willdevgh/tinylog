#ifndef __EVENT__
#define __EVENT__

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace tl {

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
		std::lock_guard<std::mutex> lock(_mutex);
		_condition.wait(_mutex);
	};
	
	void timed_wait(int sec)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_condition.wait_for(_mutex, std::chrono::seconds(sec));
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
	std::mutex _mutex;
	std::condition_variable_any _condition;
};

} // namespace tl

#endif // __EVENT__
