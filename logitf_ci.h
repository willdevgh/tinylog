#ifdef __LOGITF_SI__
	#error "include header error!!"
#endif

#ifndef __LOGITF_CI__
#define __LOGITF_CI__

#include "tinylog.h"
using namespace tl;

// 由调用者给出日志对象ins
#define LOGDEBUG(ins, fmt, ...) ins.write(__FILE__, __LINE__, LL_DEBUG, fmt, ##__VA_ARGS__)
#define LOGINFO(ins, fmt, ...) ins.write(__FILE__, __LINE__, LL_INFO, fmt, ##__VA_ARGS__)
#define LOGERROR(ins, fmt, ...) ins.write(__FILE__, __LINE__, LL_ERROR, fmt, ##__VA_ARGS__)
#define LOGDATDUMP(ins, dat, bytes) ins.write(nullptr, bytes, LL_DATDUMP, dat) 
#define LOGDATDUMPMT(ins, filename, dat, bytes) ins.write(filename, bytes, LL_DATDUMP_MT, dat)

#endif // __LOGITF_CI__
