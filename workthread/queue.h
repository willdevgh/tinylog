#ifndef __QUEUE__
#define __QUEUE__

#include <deque>
#include <mutex>

namespace tl {

template<class T>
class queue: public std::deque<T>
{
public:
	queue() = default;
	~queue() = default;

	void push(const T &t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::deque<T>::push_back(t);
	}

	void push(T &&t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::deque<T>::push_back(std::move(t));
	}
	
	bool get(T &t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (!std::deque<T>::empty())
		{
			t = std::deque<T>::front();
			std::deque<T>::pop_front();
			return true;
		}
		
		return false;
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::deque<T>::clear();
	}
private:
	std::mutex _mutex;
};

} // namespace tl

#endif // __QUEUE__
