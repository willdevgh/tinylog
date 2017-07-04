#include "tinylog.h"
#include <iostream>
#include <string>


int _tmain(void)
{

	std::cout << tinylog::getdate(_T("-")) << std::endl;
	std::cout << tinylog::gettime(_T(":")) << std::endl;
	std::cout << tinylog::getdatetime(_T("-"), _T(":"), _T(" ")) << std::endl;


	tinylog log;
	log.set_max_size(1024); // max size = 1024KB
	int res = log.start(_T("test"), _T(".log"), _T("./logs"));
	std::cout << _T("res: ") << g_error_string[res]	<< std::endl;

	int i = 0;
	for (; i < 1000; ++i)
	{
		res = log.write(__FILE__, __LINE__, LL_INFO, _T("%s - %03d"), _T("wwtest"), i);
		if (res != LOGERR_OK)
			std::cout << _T("log.write res: ") << g_error_string[res] << std::endl;
	}

	std::cout << "before finish" << std::endl;
	res = log.finish();
	std::cout << _T("res: ") << g_error_string[res] << std::endl;

	return 0;
}
