#include "core.h"
#include "platform/platform.h"

#include <mach/mach_time.h>

namespace Platform
{

	static F64                          g_machTimeToNs;
	static mach_timebase_info_data_t	g_timebase;

	void Init()
	{
		mach_timebase_info(&g_timebase);
		g_machTimeToNs = ((F64)g_timebase.numer / (F64)g_timebase.denom);
	}

	void Shutdown()
	{
	}

	void LogError(const char* str)
	{
		printf("%s", str);
	}

	void LogInfo(const char* str)
	{
		printf("%s", str);
	}

	void LogDebug(const char* str)
	{
		printf("%s", str);
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

