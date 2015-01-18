#ifndef PLATFORM_H
#define PLATFORM_H

namespace Platform
{
	void		Init();
	void		Shutdown();

	void		LogError(const char* str);
	void		LogInfo(const char* str);
	void		LogDebug(const char* str);

	F64			GetSystemTicksToSecondsScale();
	U64			GetSystemTicks();
}

#endif
