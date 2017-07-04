/* 
unit test for queue.h
g++ -o tq testqueue.cc queue.h -std=c++11 -Wall
*/


#include <iostream>
#include <string>
#include <string.h>
#include "queue.h"

using namespace std;

int main(void)
{
	queue<string> squeue;
	queue<int> iqueue;

	int i = 0, max_queue_size = 10;
	char buff[16];
	for (; i < max_queue_size; ++i)
	{
		memset(buff, 0x00, sizeof(buff));
		sprintf(buff, "test_%02d", i);
		squeue.push(buff);
		iqueue.push(i);
	}

	for (auto ival : iqueue)
	{
		cout << "ival: " << ival << endl;
	}

	cout << endl;
	
	for (auto sval : squeue)
	{
		cout << "sval: " << sval << endl;
	}

	cout << "===========================" << endl;
	iqueue.clear();
	squeue.clear();

	for (auto ival : iqueue)
	{
		cout << "ival: " << ival << endl;
	}

	cout << endl;
	
	for (auto sval : squeue)
	{
		cout << "sval: " << sval << endl;
	}

	return 0;
}
