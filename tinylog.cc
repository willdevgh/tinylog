#include "tinylog.h"
#include "loghandler.h"

const TCHAR * g_error_string[LOGERR_TOTALNUM] = {
_T("LOGERR_OK"),
_T("LOGERR_PARAM"),
_T("LOGERR_ALREADYSTART"),
_T("LOGERR_NOTSTART"),
_T("LOGERR_OPEN"),
_T("LOGERR_CLOSE"),
_T("LOGERR_ALREADYCLOSE"),
_T("LOGERR_FILENOTFOUND"),
_T("LOGERR_DISABLE")
};

// ********** private functions **********
tstring tinylog::getdate(const TCHAR *sep /*= _T("")*/)
{
    tstring separator;

    if (_tcslen(sep) > 1)
        separator = _T("-");
    else
        separator = sep;

    time_t ti = time(nullptr);
	struct tm *pt = localtime(&ti);
    TCHAR date_buff[32];
    memset(date_buff, 0x00, sizeof date_buff);

	_stprintf(date_buff, _T("%04d%s%02d%s%02d"), 
		pt->tm_year + 1900, separator.c_str(), pt->tm_mon + 1, separator.c_str(), pt->tm_mday);
	
    return date_buff;
}

tstring tinylog::gettime(const TCHAR *sep /*= _T("")*/)
{	
    tstring separator;

    if (_tcslen(sep) > 1)
        separator = _T(":");
    else
        separator = sep;

    time_t ti = time(nullptr);
	struct tm *pt = localtime(&ti);
    TCHAR time_buff[32];
    memset(time_buff, 0x00, sizeof time_buff);

	_stprintf(time_buff, _T("%02d%s%02d%s%02d"), 
		pt->tm_hour, separator.c_str(), pt->tm_min, separator.c_str(), pt->tm_sec);
	
	return time_buff; 
}

tstring tinylog::getdatetime(const TCHAR *dsep /*= _T("")*/, const TCHAR *tsep /*= _T("")*/, const TCHAR *dtsep /*= _T(" ")*/)
{
    tstring dseparator, tseparator, dtseparator;

    if (_tcslen(dsep) > 1)
        dseparator = _T("-");
    else
        dseparator = dsep;

    if (_tcslen(tsep) > 1)
        tseparator = _T(":");
    else
        tseparator = tsep;

    if (_tcslen(dtsep) == 0)
        dtseparator = _T(" ");
    else
        dtseparator = dtsep;

    time_t ti = time(nullptr);
	struct tm *pt = localtime(&ti);
    TCHAR datetime_buff[32];

    memset(datetime_buff, 0x00, sizeof datetime_buff);

	_stprintf(datetime_buff, _T("%04d%s%02d%s%02d%s%02d%s%02d%s%02d"), 
		pt->tm_year + 1900, dseparator.c_str(), pt->tm_mon + 1, dseparator.c_str(), pt->tm_mday,
		dtsep,
		pt->tm_hour, tseparator.c_str(), pt->tm_min, tseparator.c_str(), pt->tm_sec);
	
	return datetime_buff;
}

tstring tinylog::makeindex(int idx)
{
	if (idx > 9999)
		return _T("");

	TCHAR idx_buff[8];
	memset(idx_buff, 0x00, sizeof idx_buff);
	
	_stprintf(idx_buff, _T("%04d"), idx);

	return idx_buff;
}

tstring tinylog::getindex()
{
	tstring log_index;
	struct dirent *pde = nullptr;
    struct stat filestat;
    TCHAR filename[256];
    DIR *pd = opendir(_path.c_str());
    if (pd == nullptr)
    {
		//printf("opendir error: [%s]\n", strerror(errno));
        return _T("");
    }
	
	tstring curr_date = getdate();
	tstring temp_name, temp_index;
	int idx = 0, max_index = 0, curr_index = 0;
    while((pde = readdir(pd)) != nullptr)
    {
		memset(filename, 0x00, sizeof filename);
		_stprintf(filename, _T("%s/%s"), _path.c_str(), pde->d_name);
		temp_name = pde->d_name;
		stat(filename, &filestat);
		
		if(S_ISREG(filestat.st_mode) == false)
			continue;
		
		if (temp_name.find(curr_date) == tstring::npos)
			continue;

		idx = (int)temp_name.rfind(_T("_"));
		temp_index = temp_name.substr(idx + 1, 4);
		curr_index =  _ttoi(temp_index.c_str());

		if (curr_index > max_index)
		{
			max_index = curr_index;
			log_index = temp_index;
		}
    }

	closedir(pd);
	pd = nullptr;

	return log_index;
}

tstring tinylog::makefullpathname(const TCHAR *date, const TCHAR *idx)
{
	TCHAR fullpathname[TL_FILENAME_MAX];
	memset(fullpathname, 0x00, sizeof fullpathname);
	_stprintf(fullpathname, _T("%s/%s_%s_%s%s"),
		_path.c_str(), _base_name.c_str(), date, idx, _ext.c_str());
	
	return fullpathname;
}

long tinylog::getlogsize(const TCHAR *logfile)
{
	struct stat st;
	if (stat(logfile, &st) < 0)
		return -1;
	
	return (long)(st.st_size/1024);
}

tstring tinylog::loglevelstr(LOGLV lv)
{
	switch (lv)
	{
	case LL_DEBUG:
		return _T("DEBUG");
	case LL_INFO:
		return _T("INFO");
	case LL_ERROR:
		return _T("ERROR");
	case LL_HEXDUMP:
		return _T("HEXDUMP");
	default:
		return _T("XXXX");
	}
}

int tinylog::open(FILE **ppf)
{
	/*
		找到当前日期最大索引的日志文件：
		1.若没找到，则新建日志文件，索引从"0001"开始。
		2.若找到，判断文件大小：
			2.1.若小于_log_size_max，打开文件。
			2.2.若大于_log_size_max，取其索引号，创建新索引号，组建新文件名，打开新文件。
	 */
	tstring log_index = getindex();
	tstring new_index, new_filename;
	// 1.
	if (log_index.length() == 0)
	{
		new_index = makeindex(1);
		new_filename = makefullpathname(getdate().c_str(), new_index.c_str());
		if ((*ppf = _tfopen(new_filename.c_str(), _T("a"))) == nullptr)
			return LOGERR_OPEN;
		
		return LOGERR_OK;
	}
	// 2.
	tstring filename = makefullpathname(getdate().c_str(), log_index.c_str());
	long log_size = getlogsize(filename.c_str());
	if (log_size < _log_size_max)
	{
		// 2.1.
		if ((*ppf = _tfopen(filename.c_str(), _T("a"))) == nullptr)
			return LOGERR_OPEN;
		
		return LOGERR_OK;	
	}
	else
	{
		// 2.2.
		new_index = makeindex(_ttoi(log_index.c_str()) + 1);
		new_filename = makefullpathname(getdate().c_str(), new_index.c_str());
		if ((*ppf = _tfopen(new_filename.c_str(), _T("a"))) == nullptr)
			return LOGERR_OPEN;
		
		return LOGERR_OK;
	}

	return LOGERR_OK;	
}

int tinylog::close(FILE *pf)
{
	if (_logfile == nullptr)
		return LOGERR_ALREADYCLOSE;

	if (fclose(_logfile) == EOF)
		return LOGERR_CLOSE;

	_logfile = nullptr;

	return LOGERR_OK;
}

int tinylog::writeline(const TCHAR *file, int line, LOGLV lv, const TCHAR *fmt, va_list arglist)
{
	if (lv == LL_HEXDUMP)
	{
		// not available yet.
	}
	else
	{
		tstring curr_time = gettime(_T(":"));
		tstring log_level = loglevelstr(lv);
		TCHAR formatbuff[MAXLINESIZE];
		memset(formatbuff, 0x00, sizeof formatbuff);
		_vsntprintf(formatbuff, MAXLINESIZE, fmt, arglist);
		formatbuff[MAXLINESIZE-1] = '\0';

		TCHAR linebuff[MAXLINESIZE*2];
		memset(linebuff, 0x00, sizeof linebuff);
		_stprintf(linebuff, _T("%s[%s][%s][%d]: %s\n"), 
			curr_time.c_str(), log_level.c_str(), file, line, formatbuff);
		
		_log_handler->put(linebuff);
	}
	
	int err = LOGERR_OK;
	if (ftell(_logfile) > _log_size_max * 1024)
	{
		_log_handler->wait4processover();
		
		err = close(_logfile);
		if (err != LOGERR_OK)
			return err;
		
		err = open(&_logfile);
		if (err != LOGERR_OK)
			return err;
	}

	return LOGERR_OK;
}

// ********** log interfaces ********** 接口
int tinylog::start(const TCHAR *base_name, const TCHAR *ext, const TCHAR *path)
{
	if (_is_start)
		return LOGERR_OK;

	if (nullptr == base_name || _tcslen(base_name) == 0)
		return LOGERR_PARAM;

	_base_name = base_name;

	(nullptr == ext || _tcslen(ext) == 0)?
		_ext = _T(".log") : _ext = ext;
		
	(nullptr == path || _tcslen(path) == 0)?
		_path = _T(".") : _path = path;

	int res = open(&_logfile);
	if (res != LOGERR_OK)
		return res;

	// ... thread
	_log_handler = new loghandler;
	_log_handler->sethandle(this);
	_log_handler->threadstart();

	
	_is_start = true;
	return LOGERR_OK;
}

int tinylog::write(const TCHAR * f, int l, LOGLV lv, const TCHAR * fmt, ...)
{
	if (!_is_start)
		return LOGERR_NOTSTART;

	if (fmt == nullptr || _tcslen(fmt) == 0 || lv > LL_LVCNT)
		return LOGERR_PARAM;

	int err = LOGERR_OK;
	va_list arglist;

	va_start(arglist, fmt);
	err = writeline(f, l, lv, fmt, arglist);
	va_end(arglist);
 
	return err;
}

int tinylog::hexdump(const TCHAR * dat, int len)
{
	return LOGERR_OK;
}

int tinylog::finish()
{
	if (!_is_start)
		return LOGERR_NOTSTART;
	// step0. set flag variable '_is_start' to FALSE.
	_is_start = false;	
	// step1. wait until thread exit.
	_log_handler->threadexit();
	// step2. close file.
	int res = close(_logfile);
	// step3. delete handler.
	delete _log_handler;
	_log_handler = nullptr;

	return res;
}


// END OF FILE: tinylog.cc