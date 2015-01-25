#include "core.h"

#include "timeutil.h"

#include <stdarg.h>
#include <string.h>

#define PLATFORM_FORMAT_LOG_MESSAGE(fn, level)  \
	char buf[16*1024]; \
	if (g_coreLogTimeEnabled) \
		{ \
		sprintf(buf, "[%.2f] ", g_coreAppTimer->GetTimeSeconds()); \
		fn(buf); \
		} \
    va_list ap; \
    va_start(ap, fmt); \
	vsprintf(buf, fmt, ap); \
	va_end(ap); \
	fn(buf);

static bool		g_coreLogTimeEnabled;
static Timer*	g_coreAppTimer;

//-----------------------------------------------------------------------------------------------------------------------------------------

void _mbLogSetTimeEnabled(bool b)
{
	g_coreLogTimeEnabled = b;
}

void _mbLogErrorfNewLine(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(Platform::LogErrorNewLine, 0);
}

void _mbLogInfofNewLine(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(Platform::LogInfoNewLine, 0);
}

void _mbLogDebugfNewLine(const char* fmt, ...)
{
	PLATFORM_FORMAT_LOG_MESSAGE(Platform::LogDebugNewLine, 0);
}

void Core_Init()
{
    Platform::Init();
    
    g_coreAppTimer = new Timer();
}

void Core_Shutdown()
{
    Platform::Shutdown();
    
    delete g_coreAppTimer;
}

double Core_GetElapsedTimeSeconds()
{
    return g_coreAppTimer->GetTimeSeconds();
}
