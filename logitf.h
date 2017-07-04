#ifndef __LOGITF__
#define __LOGITF__

#include "tinylog.h"

#define LOGDEBUG(fmt, ...) g_log.write(__FILE__, __LINE__, LL_DEBUG, fmt, ##__VA_ARGS__)
#define LOGINFO(fmt, ...) g_log.write(__FILE__, __LINE__, LL_INFO, fmt, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) g_log.write(__FILE__, __LINE__, LL_ERROR, fmt, ##__VA_ARGS__)

extern tinylog g_log;

#endif // __LOGITF__
