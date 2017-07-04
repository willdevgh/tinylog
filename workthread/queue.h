#ifndef __QUEUE__
#define __QUEUE__

#include <deque>
#include "lock.h"


template<class T>
class queue: public std::deque<T>
{
public:
	queue() = default;
	~queue() = default;

	void push(const T &t)
	{
		scopedlock slock;
		std::deque<T>::push_back(t);
	}

	void push(T &&t)
	{
		scopedlock slock;
		std::deque<T>::push_back(std::move(t));
	}
	
	bool get(T &t)
	{
		scopedlock slock;
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
		scopedlock slock;
		std::deque<T>::clear();
	}
private:
	// nothing for now.
};



#endif // __QUEUE__
