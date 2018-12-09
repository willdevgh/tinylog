/* 
test-mt: 多线程写日志测试
*/

#include <iostream>
#include <string>
#include <thread>
#include "logitf_si.h"
#include "tinylog.h"

const TCHAR *datfile = _T("test");
static long getsize(const TCHAR *filename)
{
	struct stat st;
	if (stat(filename, &st) < 0)
		return -1;
	
	return (long)(st.st_size);
}

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
		
		// specify an 'i' value to test LOGDATDUMP:
		if (i == 1234)
		{
			std::cout << std::endl << _T("BEGIN!!!") << std::endl;
			long size = getsize(datfile);
			if (size == -1)
			{
				std::cout << _T("size = -1") << std::endl;
				continue;
			}

			FILE *file = fopen(datfile, "r");
			unsigned char * buff = new unsigned char[size + 1];
			fread(buff, 1, size, file);
			buff[size] = 0x00;
			fclose(file);
			LOGDATDUMPMT(_T("file1234"), buff, size);
			delete[] buff;
			buff = nullptr;
			std::cout << _T("DUMP DONE!") << std::endl << std::endl;
		}
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
	std::cout << _T("main() start...") << std::endl;

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
	std::cout << _T("main() exit.") << std::endl;
	return 0;
}

