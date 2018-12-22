#ifndef __TINYLOG__
#define __TINYLOG__

#include <sstream>
#include <string>
#include <memory>
#include <mutex>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

# include <dirent.h>
# include <libgen.h>
# include <sys/stat.h>
# include <stddef.h>

//#if defined  __linux__
# include <linux/limits.h>

// following 2 header files are copied from: 
// http://www.cnblogs.com/zyl910/archive/2013/01/17/tcharall.html
#include "autot/auto_tchar.h"
#include "autot/prichar.h"


namespace tl {

const int TL_PATH_MAX = PATH_MAX;
//#else
//const int TL_PATH_MAX = 4096;
//#endif

const int MAXLINESIZE = 1024;

class loghandler;
using tstring = std::basic_string<TCHAR>;
using tstringstream = std::basic_stringstream<TCHAR>;
using byte = unsigned TCHAR;

const int TL_FILENAME_MAX = 256;
const int TL_FILEEXT_MAX = 3;

// error code
enum LOGERR
{
    LOGERR_OK = 0,
    LOGERR_PARAM,
    LOGERR_ALREADYSTART,
    LOGERR_NOTSTART,
    LOGERR_OPEN,
    LOGERR_CLOSE,
    LOGERR_ALREADYCLOSE,
    LOGERR_FILENOTFOUND,
    LOGERR_DISABLE,
	LOGERR_TOTALNUM // total number of error codes.
};

extern const TCHAR * g_error_string[LOGERR_TOTALNUM];

// log level
enum LOGLV
{
	LL_DEBUG = 0,
	LL_INFO,
	LL_ERROR,
	LL_DATDUMP,
	LL_DATDUMP_MT,
	LL_LVCNT
};

class tinylog final
{
public:
	tinylog() {};
	~tinylog() 
	{
		//finish();
	};

	friend class loghandler;
	// log interfaces
	int start(const TCHAR *base_name, const TCHAR *ext, const TCHAR *path);
	int write(const TCHAR *f, int l, LOGLV lv, const void *input, ...);
	int finish();

	void set_max_size(long size)
	{
		if (size < 1024) // make sure >= 1MB
		{
			_log_size_max = 1024;
			return;
		}
		else if (size > 1024 * 300) // make sure <= 300MB
		{
			_log_size_max = 1024 * 300;
			return;
		}

		_log_size_max = size;
	};

	int is_start() const { return _is_start; };

	static tstring getdate(const TCHAR *sep = _T(""));
	static tstring gettime(const TCHAR *sep = _T(""));
	static tstring getdatetime(const TCHAR *dsep = _T(""), const TCHAR *tsep = _T(""), const TCHAR *dtsep = _T(" "));
private:
	tstring getindex();
	tstring makeindex(int idx);
	tstring getindex(const TCHAR *filename);
	tstring getbasename(const TCHAR *filename);
	tstring makefullpathname(const TCHAR *date, const TCHAR *idx); 
	long getlogsize(const TCHAR *logfile); // KB
	tstring loglevelstr(LOGLV lv);
	int open(FILE **ppf);
	int close(FILE *pf);
	int writeline(const TCHAR *file, int line, LOGLV lv, const void *fmt, va_list arglist);
	int dumpline(FILE *file, unsigned int addr, const void *buf, int bytes);
	int dumphex(const TCHAR *hexfile, const void *data, int bytes);
	int writedump(FILE *file, const void *data, int bytes);
	tstring makehexfilename(const TCHAR *hexfile);

private:
	loghandler *_log_handler = nullptr;
	bool _is_start = false;
	//bool _is_dumping = false;
	tstring _base_name;
	tstring _ext;
	tstring _path;
	tstring _curr_file;
	long _log_size_max = 1024 * 5; // 5MB for default log size.
	FILE *_logfile = nullptr;

	std::mutex _mut;
};

} // namespace tl

#endif /* __TINYLOG__ */
// END OF FILE: tinylog.h
