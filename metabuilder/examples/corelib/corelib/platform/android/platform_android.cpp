#include "corelib/core.h"
#include "corelib/platform/platform.h"

#include <android/log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fnmatch.h>
#include <dirent.h>
#include <fcntl.h>

namespace Platform
{
	void Init()
	{
	}

	void Shutdown()
	{
	}

	void LogErrorNewLine(const char* str)
	{
		__android_log_print(ANDROID_LOG_ERROR, "mobileapp", "%s", str);
	}

	void LogInfoNewLine(const char* str)
	{
		__android_log_print(ANDROID_LOG_INFO, "mobileapp", "%s", str);
	}

	void LogDebugNewLine(const char* str)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "mobileapp", "%s", str);
	}

	F64 GetSystemTicksToSecondsScale()
	{
		return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS);
	}

	U64 GetSystemTicks()
	{
		timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		U64 nanoseconds = (U64)t.tv_nsec;
		U64 seconds = (U64)t.tv_sec;
		nanoseconds += seconds * MB_SECONDS_TO_NANOSECONDS;
		return nanoseconds;
	}
}
