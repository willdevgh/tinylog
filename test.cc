/* test */

#include <iostream>
#include <string>
#include "logitf.h"
#include "tinylog.h"


const TCHAR *datfile = _T("test-mt");
static long getsize(const TCHAR *filename)
{
	struct stat st;
	if (stat(filename, &st) < 0)
		return -1;
	
	return (long)(st.st_size);
}


int _tmain(void)
{
	std::cout << tinylog::getdate(_T("-")) << std::endl;
	std::cout << tinylog::gettime(_T(":")) << std::endl;
	std::cout << tinylog::getdatetime(_T("-"), _T(":"), _T(" ")) << std::endl;

	g_log.set_max_size(1024); // max size = 1024KB
	int res = g_log.start(_T("test"), _T(".log"), _T("./logs"));
	std::cout << _T("res: ") << g_error_string[res]	<< std::endl;

	int i = 0;
	for (; i < 1000; ++i)
	{
		res = LOGINFO(_T("%s - %03d"), _T("wwtest"), i);
		if (res != LOGERR_OK)
			std::cout << _T("log.write res: ") << g_error_string[res] << std::endl;

		if (i == 666)
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
			LOGDATDUMP(buff, size);
			delete[] buff;
			buff = nullptr;
			std::cout << _T("DUMP DONE!") << std::endl << std::endl;
		}
	}

	std::cout << _T("before finish") << std::endl;
	res = g_log.finish();
	std::cout << _T("res: ") << g_error_string[res] << std::endl;

	return 0;
}



