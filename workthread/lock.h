#ifndef __LOCK__
#define __LOCK__

//#include <memory>
#include <thread>
#include <mutex>

class simplelock final
{
public:
	simplelock() = default;
	~simplelock() = default;
	simplelock(const simplelock&) = delete;
	simplelock(simplelock&&) = delete;
	simplelock& operator=(simplelock&) = delete;
	simplelock& operator=(simplelock&&) = delete;

	void lock() { _mutex.lock(); };
	void unlock() { _mutex.unlock(); };
	 
private:
	std::mutex _mutex;
};


class scopedlock final
{
public:
	scopedlock()
	{
		_mutex.lock();
	};
	~scopedlock()
	{
		_mutex.unlock();
	};
	scopedlock(const scopedlock&) = delete;
	scopedlock(scopedlock&&) = delete;
	scopedlock& operator=(scopedlock&) = delete;
	scopedlock& operator=(scopedlock&&) = delete;

	void lock() { _mutex.lock(); };
	void unlock() { _mutex.unlock(); };
		
private:
	std::mutex _mutex;
};

#endif // __LOCK__
