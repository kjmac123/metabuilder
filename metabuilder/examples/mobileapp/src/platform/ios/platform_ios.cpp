#include "core.h"
#include "platform/platform.h"

#include <android/log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fnmatch.h>
#include <dirent.h>
#include <fcntl.h>

#if defined(PLATFORM_OSX)
#include <mach/mach_time.h>
#endif

namespace Platform
{

#if defined(PLATFORM_OSX)
	static F64				g_machTimeToNs;
	static mach_timebase_info_data_t	g_timebase;
#endif

	void Init()
	{
#if defined(PLATFORM_OSX)
		mach_timebase_info(&g_timebase);
		g_machTimeToNs = ((F64)g_timebase.numer / (F64)g_timebase.denom);
#endif
	}

	void Shutdown()
	{
	}

	void LogError(const char* str)
	{
		printf("%s", str);
		__android_log_print(ANDROID_LOG_ERROR, "metabuilder", "%s", str);
	}

	void LogInfo(const char* str)
	{
		printf("%s", str);
		__android_log_print(ANDROID_LOG_INFO, "metabuilder", "%s", str);
	}

	void LogDebug(const char* str)
	{
		printf("%s", str);
		__android_log_print(ANDROID_LOG_DEBUG, "metabuilder", "%s", str);
	}

	F64 GetSystemTicksToSecondsScale()
	{
#if defined(PLATFORM_OSX)
		return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS) * g_machTimeToNs;
#else
		return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS);
#endif
	}

	U64 GetSystemTicks()
	{
#if defined(PLATFORM_OSX)
		return mach_absolute_time();
#else
		timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		U64 nanoseconds = (U64)t.tv_nsec;
		U64 seconds = (U64)t.tv_sec;
		nanoseconds += seconds * MB_SECONDS_TO_NANOSECONDS;
		return nanoseconds;
#endif
	}

}

