#ifndef __TINYLOG_DATAHANDLER__
#define __TINYLOG_DATAHANDLER__

#include <unistd.h>

#include "autot/auto_tchar.h"
#include "workthread/handler.h"
#include "tinylog.h"

class loghandler: public handler<tstring>
{
public:
	loghandler() = default;
	virtual ~loghandler() {};

	void sethandle(tinylog *log) { _ptr_log = log; };

	void handler_func(const tstring &ts) override
	{
		if (_ptr_log != nullptr && 
			_ptr_log->_logfile != nullptr)
		{
			_ftprintf(_ptr_log->_logfile, ts.c_str());
			fflush(_ptr_log->_logfile);
		}
		else
			usleep(500000); // 0.5 sec
	};
	
private:	
	tinylog *_ptr_log = nullptr;
};



#endif // __TINYLOG_DATAHANDLER__
