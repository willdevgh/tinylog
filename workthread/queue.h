#ifndef __QUEUE__
#define __QUEUE__

#include <queue>
#include <mutex>

namespace tl {

template<typename T>
class queue final
{
public:
	queue() = default;
	~queue() = default;

	void push(const T &t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(t);
	}

	void push(T &&t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(std::forward(t));
	}
	
	bool get(T &t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (!_queue.empty())
		{
			t = _queue.front();
			_queue.pop();
			return true;
		}
		
		return false;
	}

    bool empty() const
    {
        return _queue.empty();
    }

	void clear()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.clear();
	}
private:
    std::queue<T> _queue;
	std::mutex _mutex;
};

} // namespace tl

#endif // __QUEUE__
