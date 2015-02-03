#ifndef PLATFORM_H
#define PLATFORM_H

#define PLATFORM_MAX_PATH 512

namespace Platform
{
	void		Init();
	void		Shutdown();

	bool		LogAutoNewLine();
	void		LogErrorNewLine(const char* str);
	void		LogInfoNewLine(const char* str);
	void		LogDebugNewLine(const char* str);

	F64			GetSystemTicksToSecondsScale();
	U64			GetSystemTicks();
}

#endif
