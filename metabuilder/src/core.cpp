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
	if (g_coreLogTimeEnabled) \
	{ \
		sprintf(buf, "[%.2f] ", g_coreAppTimer.GetTimeSeconds()); \
		fn(buf); \
	} \
    va_list ap; \
    va_start(ap, fmt); \
	vsprintf(buf, fmt, ap); \
	va_end(ap); \
	strcat(buf, "\n"); \
	fn(buf);

static bool		g_coreLogTimeEnabled;
static Timer	g_coreAppTimer;

//-----------------------------------------------------------------------------------------------------------------------------------------

void _mbLogSetTimeEnabled(bool b)
{
	g_coreLogTimeEnabled = b;
}

void _mbLogErrorf(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(Platform::LogError, 0);
}

void _mbLogInfof(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(Platform::LogInfo, 0);
}

void _mbLogDebugf(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(Platform::LogDebug, 0);
}

void _mbLogErrorfLF(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE_LF(Platform::LogError, 0);
}

void _mbLogInfofLF(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE_LF(Platform::LogInfo, 0);
}

void _mbLogDebugfLF(const char* fmt, ...)
{
	if (mbGetAppState()->cmdSetup.verbose)
	{
		PLATFORM_FORMAT_LOG_MESSAGE_LF(Platform::LogDebug, 0);
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
