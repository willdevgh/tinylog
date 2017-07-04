/* 
unit test code for event.h 
g++ -o te event.h testevent.cc -std=c++11 -Wall -pthread
*/

#include <iostream>
#include <thread>
#include <unistd.h>

#include "event.h"

event g_event;

void thread_func(int max)
{
	std::cout << "thread start..." << std::endl;

	int i = 1;
	while(i < max)
	{
		g_event.wait();
		
		std::cout << "test " << i++ << std::endl;
	}

	std::cout << "thread exit." << std::endl;
}

int main(void)
{
	const int max = 8;
	std::thread thr(thread_func, max);
	
	int i = 0;
	for (; i < 8; ++i)
	{
		sleep(i+1);
		g_event.notify_one();
	}

	thr.join();
	
	std::cout << "main thread exit!" << std::endl;
}

