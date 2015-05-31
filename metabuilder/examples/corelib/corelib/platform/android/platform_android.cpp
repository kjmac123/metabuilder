#include "corelib/core.h"
#include "corelib/platform/platform.h"

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fnmatch.h>
#include <dirent.h>
#include <fcntl.h>

#include <jni.h>
#include "jnihelpers.h"

namespace Platform
{
    AAssetManager* g_assetManager;
    
	void Init(void* infoPtr)
	{
        JNIInitInfo* initInfo = static_cast<JNIInitInfo*>(infoPtr);
        MB_ASSERT(infoPtr);
        g_assetManager = AAssetManager_fromJava(initInfo->env, initInfo->assetManager);
        MB_ASSERT(g_assetManager);
        MB_LOGINFO("Platform::Init complete");
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
