#include "tinylog.h"
#include "loghandler.h"
#include <iostream>

namespace tl {

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
    memset(date_buff, 0x00, sizeof(date_buff));

	_sntprintf(date_buff, sizeof(date_buff)/sizeof(TCHAR), _T("%04d%s%02d%s%02d"), 
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
    memset(time_buff, 0x00, sizeof(time_buff));

	_sntprintf(time_buff, sizeof(time_buff)/sizeof(TCHAR), _T("%02d%s%02d%s%02d"), 
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

    memset(datetime_buff, 0x00, sizeof(datetime_buff));

	_sntprintf(datetime_buff, sizeof(datetime_buff)/sizeof(TCHAR), _T("%04d%s%02d%s%02d%s%02d%s%02d%s%02d"), 
		pt->tm_year + 1900, dseparator.c_str(), pt->tm_mon + 1, dseparator.c_str(), pt->tm_mday,
		dtsep,
		pt->tm_hour, tseparator.c_str(), pt->tm_min, tseparator.c_str(), pt->tm_sec);
	
	return datetime_buff;
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
	
	tstring curr_base_name, curr_date = getdate();
	tstring temp_name, temp_index;
	int idx = 0, max_index = 0, curr_index = 0;
    while((pde = readdir(pd)) != nullptr)
    {
		memset(filename, 0x00, sizeof(filename));
		_sntprintf(filename, sizeof(filename)/sizeof(TCHAR), _T("%s/%s"), _path.c_str(), pde->d_name);
		temp_name = pde->d_name;
		stat(filename, &filestat);
		
		if(S_ISREG(filestat.st_mode) == false)
			continue;
		
		if (temp_name.find(curr_date) == tstring::npos)
			continue;

		curr_base_name = getbasename(temp_name.c_str());
		if (curr_base_name != _base_name)
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

tstring tinylog::makeindex(int idx)
{
	if (idx > 9999)
		return _T("");

	TCHAR idx_buff[8];
	memset(idx_buff, 0x00, sizeof idx_buff);
	
	_sntprintf(idx_buff, sizeof(idx_buff)/sizeof(TCHAR), _T("%04d"), idx);

	return idx_buff;
}

tstring tinylog::getindex(const TCHAR *filename)
{
	tstring sfilename = filename;
	int idx = (int)sfilename.rfind(_T("_"));
	return sfilename.substr(idx + 1, 4);
}

tstring tinylog::getbasename(const TCHAR *filename)
{
	tstring sfilename = filename;
	int idx = (int)sfilename.find(_T("_"));
	return sfilename.substr(0, idx);
}

tstring tinylog::makefullpathname(const TCHAR *date, const TCHAR *idx)
{
	TCHAR fullpathname[TL_FILENAME_MAX];
	memset(fullpathname, 0x00, sizeof fullpathname);
	_sntprintf(fullpathname, sizeof(fullpathname)/sizeof(TCHAR), _T("%s/%s_%s_%s%s"),
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
	case LL_DATDUMP:
		return _T("HEXDUMP");
	default:
		return _T("XXXX");
	}
}

int tinylog::open(FILE **ppf)
{
	/*
		找到当前日期当前日志basename的最大索引的日志文件：
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
		
		_curr_file = new_filename;
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
		
		_curr_file = filename;
		return LOGERR_OK;	
	}
	else
	{
		// 2.2.
		new_index = makeindex(_ttoi(log_index.c_str()) + 1);
		new_filename = makefullpathname(getdate().c_str(), new_index.c_str());
		if ((*ppf = _tfopen(new_filename.c_str(), _T("a"))) == nullptr)
			return LOGERR_OPEN;

		_curr_file = new_filename;		
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

int tinylog::writeline(const TCHAR *file, int line, LOGLV lv, const void *input, va_list arglist)
{
	if (lv == LL_DATDUMP)
	{
		/*
			todo: dump log 
			参数 1、5 忽略！
			缓冲区中数据不是C风格字符串，故以 line 作为数据长度传入。
			data dump -- dumphex 函数写日志过程中
			若文件大小超过 _log_size_max 不切换 保证数据在文件中连续 ！！！
		*/
		dumphex(nullptr, input, line);

		return LOGERR_OK;
	}
	else if (lv == LL_DATDUMP_MT)
	{
		/*
			参数1为dump文件名，参数5仍然忽略。
			新建文件并写入(writedump)。
			最后在该分支直接return
		*/
		
		dumphex(makehexfilename(file).c_str(), input, line);
		
		return LOGERR_OK;
	}
	
	
	tstring curr_time = gettime(_T(":"));
	tstring log_level = loglevelstr(lv);
	TCHAR formatbuff[MAXLINESIZE];
	memset(formatbuff, 0x00, sizeof(formatbuff));
	_vsntprintf(formatbuff, MAXLINESIZE, (const TCHAR *)input, arglist);
	formatbuff[MAXLINESIZE-1] = '\0';

	TCHAR linebuff[MAXLINESIZE*2];
	memset(linebuff, 0x00, sizeof linebuff);
	_sntprintf(linebuff, sizeof(linebuff)/sizeof(TCHAR), _T("%s[%s][%s][%d]: %s\n"), 
		curr_time.c_str(), log_level.c_str(), file, line, formatbuff);
	
	_log_handler->put(linebuff);


	// check if need a rotation(switch a new file to write).
	if (getlogsize(_curr_file.c_str()) > _log_size_max)
	{
		// one thread switch file and others just put in queue(without notify()).
		if (_mut.try_lock())
		{
			_log_handler->forcewait(true);

			int err = LOGERR_OK;
			err = close(_logfile);
			if (err != LOGERR_OK)
				return err;
	
			err = open(&_logfile);
			if (err != LOGERR_OK)
				return err;
			
			_mut.unlock();
			_log_handler->forcewait(false);
			_log_handler->notify();
		}
	}
	else
	{
		_log_handler->notify();
	}

	return LOGERR_OK;
}

int tinylog::dumpline(FILE *file, unsigned int addr, const void *buf, int bytes)
{
	int i, pos;
	TCHAR line[80 + 1];
	const TCHAR SPACE = _T(' ');
	memset(line, 0x00, sizeof(line));
	
	pos = _stprintf(line, _T("%08X"), addr);
	byte *membuf = (byte *)buf;
	
	// hex content:
	for (i = 0; i < 16; ++i)
	{
		if (i % 8 == 0)
			line[pos++] = SPACE;
		
		if (i < bytes)
			pos += _stprintf(line + pos, _T("%02x "), membuf[i]);
		else
			pos += _stprintf(line + pos, _T("   ")); // 3 spaces
	}

	pos += _stprintf(line + pos, _T(" |"));

	// printable content
	for (i = 0; i < bytes; ++i)
	{
		if (bytes != 16) 
		{
			line[pos++] = '-';
		}
		else 
		{
			/* FIXME:	issue #1
			该段代码在上一个版本的树莓派系统(Jessie)上可以正常运行，
			但是升级到Stretch后就会导致段错误(segmentation fault)，
			问题初步定位为输出'%'导致程序异常（通过生成的日志可以分析）。
			临时修复：可以通过屏蔽该段代码而使用下面的代码修复——
			即每次需要输出'%'时，都用'.'代替。
			*/
			//line[pos++] = _istprint(membuf[i]) ? membuf[i] : _T('.');
			
			TCHAR temp = '.';
			if (_istprint(membuf[i])) {
				if (membuf[i] == '%')
					temp = _T('.');
				else
					temp = membuf[i];
			}
			else
				temp = _T('.');
		
			line[pos++] = temp;
		}
		
	}

	pos += _stprintf(line + pos, _T("|\n"));

	if (file != nullptr)
		writedump(file, line, pos);
	else {
		tstring temp;
		temp.assign(line, pos);
		_log_handler->put_notify(temp);
	}

	return pos;
}

int tinylog::dumphex(const TCHAR *hexfile, const void *data, int bytes)
{
	int i, dump_len = 0;
	TCHAR title[] = _T("--addr--**0--1--2--3--4--5--6--7-**8--9--a--b--c--d--e--f-*|------text------|\r\n");
	byte *membuf = (byte *)data;

	FILE *file = nullptr;
	if (hexfile != nullptr)
	{
		file = _tfopen(hexfile, _T("a"));
		writedump(file, title, (sizeof(title)/sizeof(TCHAR)) - 1); // ignore NUL
	}
	else
	{
		_log_handler->put_notify(title);
	}
	

	for (i = 0; i < (bytes / 16); ++i)
	{
		dump_len += dumpline(file, 16 * i, membuf + 16 * i, 16);
	}

	// 不足16字节的部分
	if (bytes % 16 != 0)
	{
		dump_len += dumpline(file, 16 * i, membuf + 16 * i, bytes % 16);
	}

	if (file != nullptr || hexfile != nullptr)
	{
		fclose(file);
	}

	return dump_len;
}

int tinylog::writedump(FILE *file, const void *data, int bytes)
{
	int writenbytes = (int)fwrite(data, 1, bytes, file);
	fflush(file);
	return writenbytes;
}

tstring tinylog::makehexfilename(const TCHAR *hexfile)
{
	tstringstream tss;
	tss << _log_handler->gettid();
	TCHAR finalname[128];
	memset(finalname, 0x00, sizeof finalname);

	_sntprintf(finalname, sizeof(finalname)/sizeof(TCHAR), _T("%s/%s_%s_%s.hex"),
		_path.c_str(),
		hexfile, 
		getdatetime(_T(""), _T(""), _T("")).c_str(), 
		tss.str().c_str());
	
	return finalname;
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

int tinylog::write(const TCHAR * f, int l, LOGLV lv, const void *input, ...)
{
	if (!_is_start)
		return LOGERR_NOTSTART;

	if (input == nullptr || lv > LL_LVCNT)
		return LOGERR_PARAM;

	int err = LOGERR_OK;
	va_list arglist;
	if (lv != LL_DATDUMP)
	{
		va_start(arglist, (const TCHAR *)input);
		err = writeline(f, l, lv, (const TCHAR *)input, arglist);
		va_end(arglist);
	}
	else
	{
		// 'f' and 'arglist' are not use.
		err = writeline(f, l, lv, input, arglist);
	} 
	return err;
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
	_logfile = nullptr;
	// step3. delete handler.
	delete _log_handler;
	_log_handler = nullptr;

	return res;
}

} // namespace tl

// END OF FILE: tinylog.cc
