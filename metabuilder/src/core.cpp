#include "metabuilder_pch.h"

#include "timeutil.h"

#include <stdarg.h> 

#define PLATFORM_FORMAT_LOG_MESSAGE(fn, level)  \
	char buf[16*1024]; \
    va_list ap; \
    va_start(ap, fmt); \
	vsprintf(buf, fmt, ap); \
	va_end(ap); \
	fn(buf); 

#define PLATFORM_FORMAT_LOG_MESSAGE_LF(fn, level)  \
	char buf[16*1024]; \
	sprintf(buf, core_logTimeEnabled ? "[%.2f] " : "", core_appTimer.GetTimeSeconds()); \
	fn(buf); \
    va_list ap; \
    va_start(ap, fmt); \
	vsprintf(buf, fmt, ap); \
	va_end(ap); \
	strcat(buf, "\n"); \
	fn(buf);

bool	core_logTimeEnabled;
Timer	core_appTimer;

void mbaLogError(const char* str);
void mbaLogInfo(const char* str);

//-----------------------------------------------------------------------------------------------------------------------------------------

void _mbLogSetTimeEnabled(bool b)
{
	core_logTimeEnabled = b;
}

void _mbLogErrorf(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(mbaLogError, 0);
}

void _mbLogInfof(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(mbaLogInfo, 0);
}

void _mbLogDebugf(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(mbaLogDebug, 0);
}

void _mbLogErrorfLF(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE_LF(mbaLogError, 0);
}

void _mbLogInfofLF(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE_LF(mbaLogInfo, 0);
}

void _mbLogDebugfLF(const char* fmt, ...)
{
	if (mbGetAppState()->cmdSetup.verbose)
	{
		PLATFORM_FORMAT_LOG_MESSAGE_LF(mbaLogDebug, 0);
	}
}

void ToUpperStr(char* str)
{
	for (char* cursor = str; str; ++str)
	{
		*str = toupper(*cursor);
	}
}

void ToLowerStr(char* str)
{
	for (char* cursor = str; str; ++str)
	{
		*str = tolower(*cursor);
	}
}
