#ifdef __LOGITF_CI__
	#error "include header error!!"
#endif

#ifndef __LOGITF_SI__
#define __LOGITF_SI__

#include "tinylog.h"

// 日志对象为全局对象g_log
#define LOGDEBUG(fmt, ...) g_log.write(__FILE__, __LINE__, LL_DEBUG, fmt, ##__VA_ARGS__)
#define LOGINFO(fmt, ...) g_log.write(__FILE__, __LINE__, LL_INFO, fmt, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) g_log.write(__FILE__, __LINE__, LL_ERROR, fmt, ##__VA_ARGS__)
#define LOGDATDUMP(dat, bytes) g_log.write(nullptr, bytes, LL_DATDUMP, dat) 
#define LOGDATDUMPMT(filename, dat, bytes) g_log.write(filename, bytes, LL_DATDUMP_MT, dat)

extern tl::tinylog g_log;

#endif // __LOGITF_SI__
