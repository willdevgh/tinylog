/* muti-thread testing. */

#include "logitf.h"
#include "tinylog.h"
#include <iostream>
#include <string>
#include <thread>

void threadfunc1()
{
	int i = 0;
	for (; i < 5000; ++i)
	{
		LOGINFO(_T("t1--%04d"), i);
	}
}

void threadfunc2()
{
	int i = 0;
	for (; i < 5000; ++i)
	{
		LOGINFO(_T("t2--%04d"), i);
	}	
}

void threadfunc3()
{
	int i = 0;
	for (; i < 5000; ++i)
	{
		LOGINFO(_T("t3--%04d"), i);
	}	
}

void threadfunc4()
{
	int i = 0;
	for (; i < 5000; ++i)
	{
		LOGINFO(_T("t4--%04d"), i);
	}	
}



int _tmain(void)
{
	std::cout << "main() start..." << std::endl;

	g_log.set_max_size(1024);
	g_log.start(_T("testmt"), _T(".log"), _T("./logs"));

	std::thread th1(threadfunc1);
	std::thread th2(threadfunc2);
	std::thread th3(threadfunc3);
	std::thread th4(threadfunc4);

	th1.join();
	th2.join();
	th3.join();
	th4.join();
	
	g_log.finish();
	std::cout << "main() exit." << std::endl;
	return 0;
}

