#ifndef PLATFORM_H
#define PLATFORM_H

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
