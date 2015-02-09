
#include "corelib/core.h"
#include "corelib/platform/platform.h"
#include "corelib/platform/platformfile.h"

#include <mach/mach_time.h>

namespace Platform
{

static F64                          g_machTimeToNs;
static mach_timebase_info_data_t	g_timebase;

void Init(void*)
{
	mach_timebase_info(&g_timebase);
	g_machTimeToNs = ((F64)g_timebase.numer / (F64)g_timebase.denom);
    
    File_Init();
}

void Shutdown()
{
    File_Shutdown();
}

void LogErrorNewLine(const char* str)
{
	printf("%s\n", str);
}

void LogInfoNewLine(const char* str)
{
	printf("%s\n", str);
}

void LogDebugNewLine(const char* str)
{
	printf("%s\n", str);
}

F64 GetSystemTicksToSecondsScale()
{
	return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS) * g_machTimeToNs;
}

U64 GetSystemTicks()
{
	return mach_absolute_time();
}

}
